/*
 * Copyright 2005-2009 WSO2, Inc. http://wso2.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <WSFError.h>
#include <OMElement.h>
#include <OMText.h>
#include <typeinfo>
using namespace std;
using namespace wso2wsf;

void OMElement::init(OMNode * parent, std::string localname, OMNamespace * ns)
{
    axiom_node_t * node;
    axiom_node_t * parent_c = NULL;
    axiom_namespace_t * ns_c = NULL;
    
    _default_namespace = NULL;
    _namespace = NULL;
    _parent = NULL;
    if(parent)
    {
        parent_c = parent->getAxiomNode();
    }

    if(ns)
    {
        ns_c = ns->getAxiomNamespace();
    }
    
    _wsf_axiom_element = axiom_element_create(getEnv(), parent_c, localname.c_str(), ns_c, &node);
    
    if(_wsf_axiom_element)
    {
        setAxiomNode(node);
        _parent = parent;
        if(_parent)
        {
            try
            {
                OMElement * dp = dynamic_cast<OMElement *>(_parent);
                dp->addChildLocal(this);
            }
            catch(bad_cast)
            {}
        }
        _namespace = ns;
    }
}

OMElement::OMElement(std::string localname)
{
    init(NULL, localname, NULL);
}

OMElement::OMElement(std::string localname, OMNamespace * ns)
{
    init(NULL, localname, ns);
}

OMElement::OMElement(OMNode * parent, std::string localname)
{
    init(parent, localname, NULL);
}

OMElement::OMElement(OMNode * parent, std::string localname, OMNamespace * ns)
{
    init(parent, localname, ns);
}

OMElement::OMElement(OMNode * parent, axiom_node_t * node)
{
    _default_namespace = NULL;
    _wsf_axiom_element = NULL;
    _namespace = NULL;
    _parent = NULL;

    if (node && (axiom_node_get_node_type(node, getEnv()) == AXIOM_ELEMENT))
    {
        _wsf_axiom_element = (axiom_element_t *)axiom_node_get_data_element(node, getEnv());
    }
    if (_wsf_axiom_element)
    {
        setAxiomNode(node);
        _parent = parent;
        if (_parent)
        {
            try
            {
                OMElement * dp = dynamic_cast<OMElement *>(_parent);
                dp->addChildLocal(this);
            }
            catch(bad_cast)
            {}
        }
        axiom_namespace_t * ns = axiom_element_get_namespace(_wsf_axiom_element, getEnv(), node);
        if (ns)
        {
            _namespace = new OMNamespace(axiom_namespace_get_uri(ns, getEnv()),
                axiom_namespace_get_prefix(ns, getEnv()));
        }
    }

    //Go through the children node and add them as child. 
    axiom_node_t * node_new = NULL;
    axiom_children_iterator_t * children_iterator = NULL;
    children_iterator = axiom_element_get_children(_wsf_axiom_element, getEnv(), getAxiomNode());
    if (children_iterator)
    {
        while (axiom_children_iterator_has_next(children_iterator, getEnv()))
        {
            node_new = axiom_children_iterator_next(children_iterator, getEnv());
            if(!node_new)
            {
                continue;
            }

            //we need to add only AXIOM_ELEMENT and AXIOM_TEXT
            if (axiom_node_get_node_type(node_new, getEnv()) == AXIOM_ELEMENT)
            {
                axiom_element_t * element = (axiom_element_t *)axiom_node_get_data_element(node_new, getEnv());
                if(!element)
                {
                    continue;
                }
                OMElement* element_cpp = new OMElement(this, node_new);
                _child_nodes.push_back(element_cpp);
            }
            else if (axiom_node_get_node_type(node_new, getEnv()) == AXIOM_TEXT)
            {
                axiom_text_t * text = (axiom_text_t *)axiom_node_get_data_element(node_new, getEnv());
                if(!text)
                {
                    continue;
                }
                OMText * text_cpp = new OMText(this, node_new);
                _child_nodes.push_back(text_cpp);
            }
        }
    }
}

OMElement::~OMElement()
{
    for(vector<OMAttribute *>::iterator ite = _added_attributes.begin(); 
        ite != _added_attributes.end(); ++ite)
    {
        if (getAxiomNode())
        {
            (*ite)->setAxiomAttribute(NULL);
        }
        delete *ite;
    }
    _added_attributes.clear();
   
    if (_default_namespace)
    {
        if (getAxiomNode())
        {
            _default_namespace->setAxiomNamespace(NULL);
        }
        delete _default_namespace;
    }
    if (_namespace)
    {
        delete _namespace;
    }
   for (vector<OMNamespace *>::iterator ite = _added_namespaces.begin();
            ite != _added_namespaces.end(); ++ite)
    {
        if (getAxiomNode())
        {
            (*ite)->setAxiomNamespace(NULL);
        }
        delete *ite;
    }
    _added_namespaces.clear();
}

OMNamespace * OMElement::findNamespace(std::string uri, std::string prefix)
{
    axiom_namespace_t * ns =axiom_element_find_namespace(
        _wsf_axiom_element, getEnv(), getAxiomNode(), uri.c_str(), prefix.c_str());
    if(!ns)
    {
        return NULL;
    }
    for (vector<OMNamespace *>::iterator i = _added_namespaces.begin();
        i != _added_namespaces.end(); ++i)
    {
        /* Compare Prefix first as it is generally smaller in size */
        if ((strcmp((*i)->getPrefix().c_str(), prefix.c_str()) == 0)
            && (strcmp((*i)->getURI().c_str(), uri.c_str()) == 0))
        {
            return *i;
        }
    }
    OMNamespace * ns_cpp = new OMNamespace(uri, prefix);
    _added_namespaces.push_back(ns_cpp);
    return ns_cpp;
}

OMAttribute * OMElement::getAttribute(std::string name, OMNamespace * ns)
{
    axutil_qname_t * qn = NULL;
    if (ns)
    {
        qn = axutil_qname_create(getEnv(), name.c_str(), ns->getURI().c_str(), ns->getPrefix().c_str());
    }
    else
    {
        qn = axutil_qname_create(getEnv(), name.c_str(), NULL, NULL);
    }
    axiom_attribute_t * at = axiom_element_get_attribute(_wsf_axiom_element, getEnv(), qn);
    axutil_qname_free(qn, getEnv());
    if (!at)
    {
        return NULL;
    }
    for (vector<OMAttribute *>::iterator i = _added_attributes.begin();
        i < _added_attributes.end(); i++)
    {
        /* Compare ns first as it is smaller in size */
        if (((*i)->getNamespace() == ns)
            && ((*i)->getName() == name))
        {
            return *i;
        }
    }
    OMAttribute * at_cpp = new OMAttribute(name, axiom_attribute_get_value(at, getEnv()), ns);
    _added_attributes.push_back(at_cpp);
    return at_cpp;
}

OMAttribute * OMElement::getAttribute(std::string name)
{
    return getAttribute(name, NULL);
}

bool OMElement::addAttribute(OMAttribute * attribute)
{
    axis2_status_t status = axiom_element_add_attribute(_wsf_axiom_element, getEnv(),
        attribute->getAxiomAttribute(), getAxiomNode());
    if (status == AXIS2_SUCCESS)
    {
        _added_attributes.push_back(attribute);
        return true;
    }
    return false;
}

bool OMElement::declareDefaultNamespace(std::string uri)
{
    axiom_namespace_t * ns =
        axiom_element_declare_default_namespace(_wsf_axiom_element, getEnv(), const_cast<char *>(uri.c_str()));
    if (ns)
    {
        if (_default_namespace)
        {
            _default_namespace->setAxiomNamespace(NULL);
            delete _default_namespace;
        }
        _default_namespace = new OMNamespace(axiom_namespace_get_uri(ns, getEnv()),
            axiom_namespace_get_prefix(ns, getEnv()));
        return true;
    }
    return false;
}



axiom_types_t OMElement::nodeType()
{
    return AXIOM_ELEMENT;
}

OMNode * OMElement::getPreviousSibling()
{
    OMElement * dp;
    try
    {
        dp = dynamic_cast<OMElement *>(_parent);
    }
    catch(bad_cast)
    {
        return NULL;
    }
    std::vector<OMNode *> children = dp->getChilderen();
    if (children.size() <= 1)
    {
        return NULL;
    }
    vector<OMNode *>::iterator i = children.begin();
    while((*i) != this)
    {
        if (i == children.end())
        {
            return NULL;
        }
        i++;
    }
    return *(i - 1);
}

OMNode * OMElement::getNextSibling()
{
    OMElement * dp;
    try
    {
        dp = dynamic_cast<OMElement *>(_parent);
    }
    catch(bad_cast)
    {
        return NULL;
    }
    std::vector<OMNode *> children = dp->getChilderen();
    if (children.size() <= 0)
    {
        return NULL;
    }
    vector<OMNode *>::iterator i = children.end() - 1;
    while((*i) != this)
    {
        if (i == children.begin())
        {
            return NULL;
        }
        i--;
    }
    return *(i + 1);
}

OMNode * OMElement::getLastChild()
{
    if (_child_nodes.size() <= 0)
    {
        return NULL;
    }
    return *(_child_nodes.end() - 1);
}

OMElement * OMElement::getFirstElement()
{
    for (vector<OMNode *>::iterator i = _child_nodes.begin();
        i < _child_nodes.end(); i++)
    {
        try
        {
            OMElement * dp = dynamic_cast<OMElement *>(*i);
            return dp;
        }
        catch(bad_cast)
        {}
    }
    return NULL;
}

OMNode * OMElement::getFirstChild()
{
    if (_child_nodes.size() <= 0)
    {
        return NULL;
    }
    return *_child_nodes.begin();
}

OMNode * OMElement::getParent()
{
    return _parent;
}

OMNamespace * OMElement::getNamespace(bool is_default)
{
    if (is_default)
    {
        axiom_namespace_t * ns =
            axiom_element_get_default_namespace(_wsf_axiom_element, getEnv(), getAxiomNode());
        if (!ns)
        {
            return NULL;
        }
        if (!_default_namespace)
        {
            _default_namespace = new OMNamespace(axiom_namespace_get_uri(ns, getEnv()),
                axiom_namespace_get_prefix(ns, getEnv()));
        }
        return _default_namespace;
    }
    if (_namespace)
    {
        return _namespace;
    }
    axiom_namespace_t * ns =
        axiom_element_get_namespace(_wsf_axiom_element, getEnv(), getAxiomNode());
    if(!ns)
    {
        return NULL;
    }
    _namespace = new OMNamespace(axiom_namespace_get_uri(ns, getEnv()),
        axiom_namespace_get_prefix(ns, getEnv()));
    return _namespace;
}

OMNamespace * OMElement::getNamespaceLocal(bool is_default)
{
    if (is_default)
    {
        return _default_namespace;
    }
    return _namespace;
}

bool OMElement::setNamespace(OMNamespace * ns, bool no_find)
{
    axis2_status_t status = AXIS2_FAILURE;
    if (no_find)
    {
        status = axiom_element_set_namespace_with_no_find_in_current_scope(
            _wsf_axiom_element, getEnv(), ns->getAxiomNamespace());
        if (status == AXIS2_SUCCESS)
        {
            _namespace = ns;
            return true;
        }
    }
    else
    {
        status = axiom_element_declare_namespace(
            _wsf_axiom_element, getEnv(), getAxiomNode(), ns->getAxiomNamespace());
    }
    if (status == AXIS2_SUCCESS)
    {
        _added_namespaces.push_back(ns);
        return true;
    }
    return false;
}

OMNode * OMElement::detach()
{
    axiom_node_t * node = NULL;
    if (getAxiomNode())
    {
        node = axiom_node_detach(getAxiomNode(), getEnv());
    }
    if (!node)
    {
        return NULL;
    }
    else
    {
        setAxiomNode(node);
        try
        {
            OMElement * dp = dynamic_cast<OMElement *>(_parent);
            dp->removeChildLocal(this);
            dp = NULL;
        }
        catch(bad_cast)
        {}
        return this;
    }
}

void OMElement::freeTree()
{
    if (_parent)
    {
        detach();
    }
    axiom_node_free_tree(getAxiomNode(), getEnv());
    setAxiomNode(NULL);
    if (_child_nodes.size() <= 0)
    {
        return;
    }
    std::vector<OMNode *> empty_child_node_array;
    setChilderen(empty_child_node_array);
}

bool OMElement::insertSiblingAfter(OMNode * to_insert)
{
    for (vector<OMNode *>::iterator i = _child_nodes.begin();
        i < _child_nodes.end(); i++)
    {
        if ((*i) == this)
        {
            _child_nodes.insert(i + 1, to_insert);
            return true;
        }
    }
    return false;
}

bool OMElement::insertSiblingBefore(OMNode * to_insert)
{
    for (vector<OMNode *>::iterator i = _child_nodes.begin();
        i < _child_nodes.end(); i++)
    {
        if ((*i) == this)
        {
            _child_nodes.insert(i, to_insert);
            return true;
        }
    }
    return false;
}

bool OMElement::build()
{
    axis2_status_t status =
        axiom_element_build(_wsf_axiom_element, getEnv(), getAxiomNode());
    return (status == AXIS2_SUCCESS);
}

OMElement * OMElement::getChildElement(std::string localname, OMNamespace * ns)
{
    for (vector<OMNode *>::iterator i = _child_nodes.begin();
        i < _child_nodes.end(); i++)
    {
        try
        {
            OMElement * dp = dynamic_cast<OMElement *>(*i);
            if ((strcmp(dp->getLocalname().c_str(), localname.c_str()) == 0)
                && (dp->getNamespace(false) == ns))
            {
                return dp;
            }
        }
        catch(bad_cast)
        {}
    }
    return NULL;
}



bool OMElement::addChild(OMNode * child)
{
    for (vector<OMNode *>::iterator i = _child_nodes.begin();
        i < _child_nodes.end(); i++)
    {
        if ((*i) == child)
        {
            axiom_node_t * node = NULL;
            axiom_children_iterator_t * children_iterator = NULL;
            children_iterator = axiom_element_get_children(_wsf_axiom_element, getEnv(), getAxiomNode());
            if (children_iterator)
            {
                while (axiom_children_iterator_has_next(children_iterator, getEnv()))
                {
                    node = axiom_children_iterator_next(children_iterator, getEnv());
                    if (node == child->getAxiomNode())
                    {
                        return false;
                    }
                }
            }
        }
    }
    axis2_status_t status =
        axiom_node_add_child(getAxiomNode(), getEnv(), child->getAxiomNode());
    if (status == AXIS2_SUCCESS)
    {
        addChildLocal(child);
        return true;
    }
    return false;
}

bool OMElement::removeChildLocal(OMNode * child)
{
    for (vector<OMNode *>::iterator i = _child_nodes.begin();
        i < _child_nodes.end(); i++)
    {
        if ((*i) == child)
        {
            try
            {
                OMElement * dp = dynamic_cast<OMElement *>(*i);
                dp->freeTree();
            }
            catch(bad_cast)
            {}
            delete *i;
            _child_nodes.erase(i);
            return true;
        }
    }
    return false;
}

void OMElement::addChildLocal(OMNode * child)
{
    _child_nodes.push_back(child);
}

string OMElement::getText()
{
    return axiom_element_get_text(_wsf_axiom_element, getEnv(), getAxiomNode());
}

bool OMElement::setText(std::string text)
{
    axis2_status_t status =
        axiom_element_set_text(_wsf_axiom_element, getEnv(), text.c_str(), getAxiomNode());
    return (status == AXIS2_SUCCESS);
}

string OMElement::getLocalname()
{
    return axiom_element_get_localname(_wsf_axiom_element, getEnv());
}

bool OMElement::removeAttribute(OMAttribute * attribute)
{
    for (vector<OMAttribute *>::iterator i = _added_attributes.begin();
        i < _added_attributes.end(); i++)
    {
        if ((*i) == attribute)
        {
            axis2_status_t status = axiom_element_remove_attribute(_wsf_axiom_element, getEnv(),
                attribute->getAxiomAttribute());
            if (status == AXIS2_SUCCESS)
            {
                (*i)->setAxiomAttribute(NULL);
                delete *i;
                _added_attributes.erase(i);
                return true;
            }
            return false;
        }
    }
    return false;
}

string OMElement::getAttributeValue(std::string name, OMNamespace * ns)
{
    return getAttribute(name, ns)->getValue();
}

string OMElement::getAttributeValue(std::string name)
{
    return getAttributeValue(name, NULL);
}

std::vector<OMNode *> OMElement::getChilderen()
{
    return _child_nodes;
}

void OMElement::setChilderen(std::vector<OMNode *> children)
{
    if (_child_nodes.size() > 0)
    {
        for (vector<OMNode *>::iterator i = _child_nodes.begin();
            i < _child_nodes.end(); i++)
        {
            try
            {
                OMElement * dp = dynamic_cast<OMElement *>(*i);
                dp->freeTree();
            }
            catch(bad_cast)
            {}

            (*i)->setAxiomNode(NULL);
            delete (*i);
        }
        _child_nodes.clear();
    }
    _child_nodes = children;
}
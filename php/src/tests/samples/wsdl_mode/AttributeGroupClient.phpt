--TEST--
Attribute Group
--FILE--
<?php

// PHP classes corresponding to the data types in defined in WSDL

class myDemo {
    public $demo1; // string
    public $demo2; // string
    public $demo3; // string
    public $demo4; // string
}

// define the class map
$class_map = array(
 "anyType" => "anyType", "myDemo" => "myDemo");

try {

    // create client in WSDL mode
    $client = new WSClient(array ("wsdl" =>"http://localhost/services/wsdl_mode/AttributeGroupService.php?wsdl",
        "classmap" => $class_map,
                            ));

    // get proxy object reference form client 
    $proxy = $client->getProxy();

    // create input object and set values
    $input = new myDemo();
    //TODO: fill in the class fields of $input to match your business logic
    $input->demo1 = "test1";
    $input->demo2 = "test2";
    $input->demo3 = "test3";
    $input->demo4 = "test4";

    // call the operation
    $response = $proxy->myDemo($input);
    //TODO: Implement business logic to consume $response, which is of type string

    echo $client->getLastRequest()."\n";
    echo $response."\n";

} catch (Exception $e) {
    // in case of an error, process the fault
    if ($e instanceof WSFault) {
        printf("Soap Fault: %s\n", $e->Reason);
    } else {
        printf("Message = %s\n", $e->getMessage());
    }
}
?>
--EXPECT--
<soapenv:Envelope xmlns:soapenv="http://www.w3.org/2003/05/soap-envelope"><soapenv:Header></soapenv:Header><soapenv:Body><ns1:myDemo xmlns:ns1="http://wso2.org/dyn/codegen/demo" ns1:demo3="test3" ns1:demo4="test4"><ns1:demo1>test1</ns1:demo1><ns1:demo2>test2</ns1:demo2></ns1:myDemo></soapenv:Body></soapenv:Envelope>
myDemo Object
(
    [demo1] => test1
    [demo2] => test2
    [demo3] => test3
    [demo4] => test4
)


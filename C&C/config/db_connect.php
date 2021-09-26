
<?php
    $servername = "XXX";
    $database = "XXX";
    $username = "XXX";
    $password = "XXX";

    try
    {
        $pdo = new PDO("mysql:host=XXX;dbname=XXX", $username, $password);
        // set the PDO error mode to exception
        $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    }
    catch(PDOException $e)
    {
        echo "Connection failed: " . $e->getMessage();
        die();
    }  
?> 

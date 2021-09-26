<?php

$row = 1;
if (($handle = fopen("wallets.csv", "r")) !== FALSE) 
{
  while (($data = fgetcsv($handle, 100, ",")) !== FALSE) 
  {
    $num = count($data);
    require_once("../M/DB_wallet.php");
    saveWalletDB($data[0], $data[1], $data[2]);
    echo $data[0] . "<br/>" . $data[1] . "<br/>" . $data[2] . "<br/>";
  }
  fclose($handle);
}


?>
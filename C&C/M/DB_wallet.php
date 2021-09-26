
<?php

function saveWalletDB($id, $pub, $priv)
{
    require ("db_connect.php");
    $stmt = $pdo->prepare("INSERT INTO Wallet (WalletID, Public_Key, Private_Key)
                           VALUES (:ID, :Public_Key, :Private_Key)");
    $stmt->bindParam(':ID', $id);
    $stmt->bindParam(':Public_Key', $pub);
    $stmt->bindParam(':Private_Key', $priv);
    $stmt->execute();
}

?>
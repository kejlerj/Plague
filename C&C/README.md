# Plague_server
_Server for Plague Ransomware_

This server is the C&C of the Plague Project.
It should use TOR to be anonymous.

It's role is :
- to get encrypted (by RSA algorithm) request
- to decrypt it
- to stock data sended by the ransomware in the database
- to generate the "decryptor" (the program that will decrypt the victim's files)
- to check in the blockchain if the victim paid the ransom
- to release the decryptor if the victim paid the ransom

If the victim didn't paid yet this message is showed :

![Screenshot from 2020-10-05 22-04-53](https://user-images.githubusercontent.com/24431487/95127737-c6472580-0758-11eb-843d-6e6edf352b7a.png)

If the victim had paid, this message is showed :

![Screenshot from 2020-10-05 22-05-08](https://user-images.githubusercontent.com/24431487/95126738-30f76180-0757-11eb-9286-10031df5a7ec.png)

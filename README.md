![Plague](https://socialify.git.ci/kejlerj/Plague/image?description=1&language=1&owner=1&stargazers=1&theme=Light)

# Plague

## 🚀 Features
- Ransomware that encrypt the user files with AES 256 algorithm (symetric encryption) with random key.
- The Ransomware send a request to the C&C with the AES key and some other victim informations (IP, MAC, hostname, location).
- This request is encrypted with RSA 4096 algorithm (asymetric encryption).

## Installation Steps
Compile the Ransomware files.
```
gcc src/* -I. -lcrypto -g3 -o plague
```

## 💻 Build With
The technologies used in this project are :
- C
- OpenSSL

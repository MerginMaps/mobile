# Secrets

To communicate with MerginAPI, some endpoints need to attach `api_key`. To not leak API_KEY,
the source code that returns the API_KEYS is encrypted. 

As a developer, if you want to develop against local mergin server, it is OK, but to 
work with public or dev.dev server, you need to manually decrypt the file. Decrypted file is never 
pushed to git!

The password for decryption is in out password manager.

if you want to change the secrets, decrypt, change and encrypt the file 
[openssl](http://stackoverflow.com/questions/16056135/ddg#16056298)
make sure you update the keys in password manager and in the kubernetes
manifest files.

encrypt
```
cd core/
openssl aes-256-cbc -in merginsecrets.cpp -out merginsecrets.cpp.enc -md md5
```

decrypt
```
cd core/
openssl aes-256-cbc -d -in merginsecrets.cpp.enc -out merginsecrets.cpp -md md5
```

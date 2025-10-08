from cryptography.fernet import Fernet
import base64
import hashlib

class Encryptor:
    _instance = None
    def __new__(cls):
        if not cls._instance:
            cls._instance = object.__new__(cls)
        return cls._instance
    
    def encrypt(self,paths :list[str],passphrase:str):
        f = Fernet(base64.urlsafe_b64encode(hashlib.sha256(passphrase.encode()).digest()))
        for path in paths:
            with open(path, 'rb') as file:
                original = file.read()
                encrypted = f.encrypt(original)
            with open(path, 'wb') as file:
                file.write(encrypted)


    def decrypt(self,paths:list[str], passphrase):
        f = Fernet(base64.urlsafe_b64encode(hashlib.sha256(passphrase.encode()).digest()))
        for path in paths:
            with open(path, 'rb') as file:
                encrypted = file.read()
                try:
                    original = f.decrypt(encrypted)
                except:
                    #bad passphrase
                    print("bad passphrase")
                    return 0
                    pass
            with open(path, 'wb') as file:
                file.write(original)


class Validaror:
    def IsValid(price:int) -> bool:
        pass
        #some realization

class Product:
    def __init__(self, price: int) -> None:
        if not Validaror.IsValid(price):
            raise Exception("Price validation error")
        self.price = price

#2
###############################################

class Image:
    def __init__(self):
        pass
    def save(self,image, path):
        pass
        #some realization
    def delete_duplications(self,image):
        pass
        #some realization

class ImageSettings:
    def __init__(self):
        pass
    def resize(self,height: int, width: int) -> None:
        pass
        #some realization
    def invert_colors(self) -> None:
        pass
        #some realization

class HttpManager:
    def __init__(self):
        pass
    def SetImageAsAccountPicture():
        pass
        #some realization
    def Download(url):
        pass
        #some realization


class ImageManager:
    def __init__(self):
        self.image = Image()
        self.setttings = ImageSettings()
        self.http_manager = HttpManager()

# Save + DeleteDuplicates
# SetImageAsAccountPicture + Download
# Resize + InvertColors (ImageChanger)





# class A:
#     def __init__(self):
#         pass

#     def method(self):
#         print("method of Class A")

# class B:
#     def __init__(self):
#         pass

#     def method(self):
#         print("method of Class B")

# class C(A,B):
#     def __init__(self):
#         pass
        
#     def func(self):
#         self.method()


# c = C()
# c.func()
#     # def mthod(self):
#     #     print("method of Class A")



#3
##################################################

class ILog:
    def __init__(self):
        pass

    def log(data):
        raise NotImplementedError('Method ILog.log is pure virtual')

class FileLogger(ILog):
    def __init__(self):
        super().__init__()
    
    def log(self, data):
        print(f"Log into FILE follow information: {data}")
        #some realization to log into FILE

class ConsoleLogger(ILog):
    def __init__(self):
        super().__init__()

    def log(self, data):
        print(f"Log into CONSOLE follow information: {data}")
        #some realization to log into CONSOLE

class DatabaseLogger():
    def __init__(self):
        super().__init__()

    def log(self, data):
        print(f"Log into DATABASE follow information: {data}")
        #some realization to log into DATABASE

class SMTPMailer():
    def __init__(self, log:ILog):
        super().__init__()
        self.ILog = log
    
    def log(self, data):
        self.ILog.log(data)
        #some realization to log into DATABASE
    

db_logger = FileLogger()
mailer = SMTPMailer(db_logger)
mailer.log("Some data")



#4
############################################

class AbstractEntity:
    def __init__(self):
        pass

class AccountEntity(AbstractEntity):
    def __init__(self):
        super().__init__()

class RoleEntity(AbstractEntity):
    def __init__(self):
        super().__init__()

class IRepository():
    def __init__(self):
        pass
    
    def save(self, entity:AbstractEntity):
        raise NotImplementedError('Method IRepository.save is pure virtual')
    

class AccountRepository(IRepository):
    def __init__(self):
        super().__init__()
    
    def save(entity:AccountEntity):
        pass

class RoleRepository(IRepository):
    def __init__(self):
        super().__init__()
    
    def save(entity:RoleEntity):
        pass



import os


class PathWalker:
    _instance = None
    def __new__(cls):
        if not cls._instance:
            cls._instance = object.__new__(cls)
        return cls._instance
    
    def get_paths(self,path:str) -> list[str]:
        array = []
        for dirpath, _, filenames in os.walk(path):
            for filename in filenames:
                array.append(os.path.join(dirpath, filename))
        return array
    
# lst = a.get_paths("..")
# for i in range(5):
#     with open(lst[i],'r') as f:
#         print(lst[i])
#         print(f.read())
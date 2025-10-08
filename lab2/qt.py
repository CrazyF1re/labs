import sys
from PyQt6.QtWidgets import QApplication, QMainWindow, QPushButton, QLabel, QFileDialog, QHBoxLayout, QWidget, QVBoxLayout, QLineEdit 
from PyQt6.QtCore import pyqtSlot
from pathwalker import PathWalker
from encryptor import Encryptor



class Validator:
    def check_data(self,*args) -> bool:
        for i in args:
            if not i:
                return False
        return True


class MainWindow(QMainWindow):
    def __init__(self):
    
        self.pathwalker = PathWalker()
        self.encryptor = Encryptor()
        self.validator = Validator()
        super().__init__()
        self.button = QPushButton("Выбрать папку")
        self.word = QLineEdit()
        self.encrypt_btn = QPushButton("Зашифровать")
        self.decrypt_btn = QPushButton("Расшифровать")
        self.label = QLabel("Путь/до/папки")
        self.setup_gui()
        self.encrypt_btn.clicked.connect(self.encrypt)
        self.decrypt_btn.clicked.connect(self.decrypt)
    
    def setup_gui(self):
        self.setWindowTitle("Folder Encryptor")
        self.setGeometry(100, 100, 400, 200)
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        row_layout = QHBoxLayout()
        row_layout2 = QHBoxLayout()
        row_layout3 = QHBoxLayout()
        
        self.button.setFixedHeight(80)
        self.button.setStyleSheet("""
            QPushButton {
                border: 1px solid #000;
                border-radius:5px;
            }
            QPushButton:hover {
                background-color: #c5c6c7;
                
            }
        """)
        self.word.setFixedHeight(30)
        self.word.setPlaceholderText("Enter secret")
        self.encrypt_btn.setFixedHeight(50)
        self.decrypt_btn.setFixedHeight(50)
        self.button.clicked.connect(self.open_dialog)
        self.label.setFixedHeight(80)
        self.label.setStyleSheet("""
            QLabel {
                font-size: 14px;
                padding: 10px;
                background-color: #f0f0f0;
                border: 1px solid #ccc;
                border-radius: 5px;
            }
        """)
        row_layout.addWidget(self.button)
        row_layout.addWidget(self.label)
        row_layout2.addWidget(self.word)
        row_layout3.addWidget(self.encrypt_btn)
        row_layout3.addWidget(self.decrypt_btn)
        main_layout.addLayout(row_layout)
        main_layout.addLayout(row_layout2)
        main_layout.addLayout(row_layout3)

    def validate(self, *args):
        return self.validator.check_data(args)
    
    def walk(self):
        self.path_list = self.pathwalker.get_paths(self.folder_path)
    

    @pyqtSlot()
    def encrypt(self):
        path = self.folder_path
        passphrase = self.word.text()
        if self.validate(path,passphrase):
            self.walk()
            self.encryptor.encrypt(self.path_list,passphrase)

    @pyqtSlot()
    def decrypt(self):
        path = self.folder_path
        passphrase = self.word.text()
        if self.validate(path,passphrase):
            self.walk()
            self.encryptor.decrypt(self.path_list,passphrase)
        

    @pyqtSlot()
    def open_dialog(self):
        fname = QFileDialog.getExistingDirectory(
            self,
            "Select Folder",
            "",
            QFileDialog.Option.ShowDirsOnly
        )
        self.folder_path = fname
        if self.folder_path == '':
            self.button.setStyleSheet("background-color: red;")    
            return
        self.button.setStyleSheet("background-color: green;")
        self.label.setText(self.folder_path)
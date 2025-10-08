import sys
from qt import MainWindow
from PyQt6.QtWidgets import QApplication

def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    app
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
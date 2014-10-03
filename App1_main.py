# -*- coding: utf-8 -*-
from PyQt4.QtGui import *
from PyQt4.QtCore import *
import os,sys
import serial
import pyqtgraph as pg 
import numpy as np 
from app1 import *

class myApp(QWidget,Ui_Form):
    def __init__(self,parent=None):
        QWidget.__init__(self)
        self.setupUi(parent)
        self.compt=0
        self.connect(self.pushButton_Init, SIGNAL("clicked()"), self.pushButton_Init_Clicked)
        self.connect(self.pushButton_Stop, SIGNAL("clicked()"), self.pushButton_Stop_Clicked)
        self.connect(self.pushButton_Send, SIGNAL("clicked()"), self.pushButton_Send_Clicked)
        self.connect(self.pushButton_Image, SIGNAL("clicked()"), self.pushButton_Image_Clicked)

        labelStyle = {'color': '#00F', 'font-size': '10pt'} 

        # 1st graph set
        self.graph_Speed.setBackgroundBrush(QBrush(QColor(Qt.white))) 
        self.graph_Speed.getAxis('bottom').setLabel('Time', units='ms',**labelStyle)
        self.graph_Speed.getAxis('left').setLabel('Speed', units='mm/ms',**labelStyle)
        self.graph_Speed.showGrid(x=True,y=True)
        # 2nd graph set
        self.graph_Deplace.setBackgroundBrush(QBrush(QColor(Qt.white))) 
        self.graph_Deplace.getAxis('bottom').setLabel('Time', units='ms',**labelStyle)
        self.graph_Deplace.getAxis('left').setLabel('Depalce', units='mm',**labelStyle)
        self.graph_Deplace.showGrid(x=True,y=True)
        #self.nombreValeurs=400

        self.timer_Serial=QTimer()
        self.connect(self.timer_Serial,SIGNAL("timeout()"),self.read)
        self.timer_Plot=QTimer()
        self.connect(self.timer_Plot, SIGNAL("timeout()"), self.plot)

        self.serialPort=None

    def pushButton_Init_Clicked(self):
        print("pushButton_Init_Clicked")

        if self.serialPort:
            self.serialPort.close()
        if self.comboBox_Port.currentText()=="":
            strPortInit="/dev/tty.usbmodem1421"
        else:
            strPortInit=str(self.comboBox_Port.currentText())
        strBaudInit=str(self.comboBox_Baud.currentText())
        try:
            self.serialPort=serial.Serial(strPortInit,strBaudInit,timeout=2)
            self.serialPort.flushInput()
            print("ok"+strBaudInit+strPortInit)
            self.pushButton_Send.setStyleSheet(QString.fromUtf8("background-color:rgb(0,255,0);"))
            self.pushButton_Send.setText("Ready")
            self.pushButton_Stop.setStyleSheet(QString.fromUtf8("background-color:rgb(255,255,255);"))
            self.pushButton_Stop.setText(QString.fromUtf8("Stop"))
            self.pushButton_Init.setStyleSheet(QString.fromUtf8("background-color: rgb(255, 255, 255);")) # bouton en orange
            self.pushButton_Init.setText(QString.fromUtf8("Init"))
        except Exception, connecting_Error:
            print("Error initialisation/请连接Arduino")
            self.pushButton_Init.setStyleSheet(QString.fromUtf8("background-color:rgb(255,100,0);"))
            self.pushButton_Init.setText("Error")


    def pushButton_Stop_Clicked(self):
        self.timer_Plot.stop()

        print("pushButton_Stop_Clicked")

        if self.serialPort:
        	self.serialPort.close()
        	self.timer_Serial.stop()

        self.pushButton_Send.setStyleSheet(QString.fromUtf8("background-color:rgb(255,255,255);"))
        self.pushButton_Send.setText(QString.fromUtf8("Start"))
        self.pushButton_Stop.setStyleSheet(QString.fromUtf8("background-color:rgb(0,0,255);"))
        self.pushButton_Stop.setText(QString.fromUtf8("Off"))
        self.pushButton_Init.setStyleSheet(QString.fromUtf8("background-color: rgb(255, 255, 255);")) # bouton en orange
        self.pushButton_Init.setText(QString.fromUtf8("Init"))

    def pushButton_Send_Clicked(self):
        self.serialPort.flushInput()
    	print("pushButton_Send_Clicked")
        print(self.pushButton_Send.text())
    	self.sP=self.horizontalSlider_sP.value()
    	print(self.sP)
    	self.sI=self.horizontalSlider_sI.value()
    	print(self.sI)
    	self.sD=self.horizontalSlider_sD.value()
    	print(self.sD)
    	self.cP=self.horizontalSlider_cP.value()
    	print(self.cP)
    	self.cI=self.horizontalSlider_cI.value()
    	print(self.cI)
    	self.cD=self.horizontalSlider_cD.value()
    	print(self.cD)
        if self.pushButton_Send.text()=="Ready":
            self.pushButton_Send.setText(QString.fromUtf8("..."))
            self.timer_Serial.start(20)
            self.timer_Plot.start(20)

        self.x = []
        #print(self.x)
        self.s=[]
        self.d=[]

        #print(self.s) 
        #print(self.d)

        self.curve_Speed=self.graph_Speed.plot(self.x,self.s, pen=(0,0,255)) 
        self.curve_Deplace=self.graph_Deplace.plot(self.x,self.d, pen=(255,0,0))


    def pushButton_Image_Clicked(self):
    	
        print("pushButton_Image_Clicked")
        if self.checkBox_Deplace.isChecked():
        	#self.x=[]
        	self.s=[]

        if self.checkBox_Speed.isChecked():
        	self.graph_Speed.setXRange(0,10)
        	self.graph_Speed.setYRange(-10,10)
        
       
    def read(self):
        print("Read_OK")
        self.char="";
      
        if self.serialPort: 
            self.timer_Serial.stop() 
            self.char=self.serialPort.readline() 
                 

            if len(self.char)>0:   
                print(self.char)  
                #self.char=self.char.strip()
                #print(self.char)
                self.data=self.char.split(" ")
                self.add(int(self.data[0]),int(self.data[1]))

            self.timer_Serial.start(20)         
     	
        


    def plot(self):
        print("Plot_OK")
        self.curve_Speed.setData(self.x,self.s)
        self.curve_Deplace.setData(self.x,self.d)
    def add(self, value_S,value_D): # function save the numbers to table 

        if self.compt==0: 
            self.points= np.array([[self.compt,value_S,value_D]]) # 3 dimension table
            self.x=self.points[:,0] 
            self.s=self.points[:,1]
            self.d=self.points[:,2]
            self.compt=self.compt+1 
        #elif self.compt<=self.nombreValeurs: 
        elif self.compt>0:
            new_S=value_S
            new_D=value_D
            self.points=np.append(self.points,[[self.compt,new_S,new_D]],axis=0)
            self.x=self.points[:,0] 
            self.s=self.points[:,1]
            self.d=self.points[:,2]

            self.compt=self.compt+1 
        # else:
        #     self.s=np.roll(self.s,-1) 
        #     self.d=np.roll(self.d,-1)
        #     self.s[self.nombreValeurs]=value_S
        #     self.d[self.nombreValeurs]=value_D

     
def main(args):
    a=QApplication(args)
    f=QWidget()
    c=myApp(f)
    f.show()
    r=a.exec_()
    return r

if __name__ == "__main__":
    main(sys.argv)
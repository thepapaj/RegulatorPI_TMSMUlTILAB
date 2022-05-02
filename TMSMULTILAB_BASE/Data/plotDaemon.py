import os 
import threading
import time
import pandas
import matplotlib
import matplotlib.pyplot as plt
import pyautogui

matplotlib.use('agg')
def view_monitor():
    minimalny_rozmiar_pliku_danych=100
    
    plik_danych='TMSLABoutputData.csv'
    plik_obrazu='plotImg.png'
    plik_widoku='plotView.html'
    poprzedni_czas_modyfikacji=0;
    
    fig=plt.figure(1,(6,6))  
   

    while True:
        time.sleep(0.2)
        
        a_window=pyautogui.getWindowsWithTitle('TMSLAB SiM')
        if len(a_window)>0: 
            a_window[0].moveTo(-100,-40)
            
        if not os.path.isfile(plik_danych):
            print('Brak pliku danych')
            continue
    
        aktualny_czas_modyfikacji=os.path.getmtime(plik_danych)
        rozmiar_pliku=os.path.getsize(plik_danych)
        if aktualny_czas_modyfikacji!=poprzedni_czas_modyfikacji \
                and rozmiar_pliku>minimalny_rozmiar_pliku_danych:
            poprzedni_czas_modyfikacji=aktualny_czas_modyfikacji
            df = pandas.read_csv(plik_danych)
            x = df['Wsp. x']
            y1 = df['Wsp. y1']
            y2 = df['Wsp. y2']
            y3 = df['Wsp. y3']
            y4 = df['Wsp. y4']
            y5 = df['Wsp. y5']
         
            plt.clf();
            plt.xticks(fontsize=14)
            plt.yticks(fontsize=14)
            plt.title("Przebieg",fontsize=14)
            plt.xlabel('Próbki[n]',fontsize=14)
            plt.ylabel("Wartosc sygnalu",fontsize=14)
            plt.plot(x, y1,color='#ff0000',label='Sygnal sterujacy',linewidth=2);
            plt.plot(x, y2,color='#0000aa',label='Temperatura wyjsciowa',linewidth=2);
            plt.plot(x, y3,color='#0ff05a',label='Zaklocenie',linewidth=2);
            plt.plot(x, y4,color='#faa900',label='Uchyb',linewidth=2);
            plt.plot(x, y5,color='#a89f47',label='Wartosc zadana',linewidth=2);
            plt.grid(color='#888888', linestyle='-')
            plt.legend(loc='upper right')
            plt.savefig(plik_obrazu)
            os.utime(plik_widoku, (aktualny_czas_modyfikacji, aktualny_czas_modyfikacji))
        
           
Thr = threading.Thread(target=view_monitor, name='view')
Thr.start()


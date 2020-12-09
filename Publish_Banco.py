import paho.mqtt.client as mqtt
import sqlite3
from sqlite3 import Error
import sys


# Chamada quando uma conexão é efetuado com sucesso no Broker
def on_connect(client, userdata, flags, rc):
    print("[STATUS] Conectado ao Broker. Resultado de conexao: " + str(rc))

#Estabelece conexão com o banco
def ConexaoBanco():
    caminho = "C:\\Users\\João\\PycharmProjects\\banco_de_dados\\comunicacao_MQTT.db"
    con = None
    try:
        con = sqlite3.connect(caminho)
    except Error as ex:
        print(ex)
    return con

# Chamado quando uma linha é adcionada ao banco
def Inserir(conexao, sql):
    try:
        c = conexao.cursor()
        c.execute(sql)
        conexao.commit()
        print("Registro inserido!")
    except Error as ex:
        print(ex)

#definicoes:
Broker = "192.168.0.122"
PortaBroker = 1883
KeepAliveBroker = 60
TopicoSubscribe = "BCIBotao1"
dispositivo = "lampada"

try:
    print("[STATUS] Inicializando MQTT...")
    # inicializa MQTT:
    client = mqtt.Client()
    client.on_connect = on_connect
    banco = sqlite3.connect('comunicacao_MQTT.db')
    var_con = ConexaoBanco()

    texto = None

    while texto != "sair":
        texto = input("comando: ")
        client.connect(Broker, PortaBroker, KeepAliveBroker)
        client.publish(TopicoSubscribe, texto)
        if texto == "0" or texto == "1":
            if texto == "0":
                var_sql = 'INSERT INTO Registros(Dispositivo_Saida, Status)VALUES("' + dispositivo + '", "on")'
                Inserir(var_con, var_sql)
            else:
                var_sql = 'INSERT INTO Registros(Dispositivo_Saida, Status)VALUES("' + dispositivo + '", "off")'
                Inserir(var_con, var_sql)

except KeyboardInterrupt:
    print("\nCtrl+C pressionado, encerrando aplicacao e saindo...")
    sys.exit(0)

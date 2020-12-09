import paho.mqtt.client as mqtt
import sys
import sqlite3
from sqlite3 import Error

#definicoes:
Broker = "192.168.0.122"
PortaBroker = 1883
KeepAliveBroker = 60
TopicoSubscribe = "Leitura_analogica"
dispositivo = "lampada"
Flag = None

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


# Chamada quando uma conexão é efetuado com sucesso no Broker
def on_connect(client, userdata, flags, rc):
    print("[STATUS] Conectado ao Broker. Resultado de conexao: " + str(rc))
    # faz subscribe automatico no topico
    client.subscribe(TopicoSubscribe)


# Callback - mensagem recebida do broker
def on_message(client, userdata, msg):
    MensagemRecebida = str(msg.payload)

    print("[MSG RECEBIDA] Topico: " + msg.topic + " / Mensagem: " + MensagemRecebida)
    var_sql = 'INSERT INTO Registros(Dispositivo_Saida, Status)VALUES("' + dispositivo + '", "' + MensagemRecebida + '")'
    Inserir(var_con, var_sql)



try:
    print("[STATUS] Inicializando MQTT...")
    # inicializa MQTT:
    client = mqtt.Client()
    client.connect(Broker, PortaBroker, KeepAliveBroker)
    client.on_connect = on_connect
    banco = sqlite3.connect('comunicacao_MQTT.db')
    var_con = ConexaoBanco()
    client.on_message = on_message

    client.loop_forever()
except KeyboardInterrupt:
    print("\nCtrl+C pressionado, encerrando aplicacao e saindo...")
    sys.exit(0)
    var_con.close()




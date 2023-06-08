import pygame
import json
import pistas
import carros
import numpy as np
import datetime
import sys
import threading
import time
# from tasks import adiciona_carro

br101 = pistas.Road()
params = json.load(open('./mock/parametros.json'))
# select a random road on params
keys = params.keys()
# CHAMAR NO DOCKER PYTHON3 SIMULADOR.PY {KEYS parametros.json}
key = sys.argv[1]
params = params[key]
plate = json.loads(open('./mock/placas.json').read())
interface_graph = params['interfaceGrafica']
path = "./data/"
pathdt = './delta-time/'
if interface_graph:
    pygame.init()
Width = 1280
Height = 720
if interface_graph:
    screen = pygame.display.set_mode((Width, Height))
if interface_graph:
    pygame.display.set_caption("Simulador de tráfego")
if interface_graph:
    bg = (10, 130, 20)
fps = 30
clock = pygame.time.Clock()
br101.setNumberLanes(params['sentido1Faixas'], params['sentido2Faixas'])
cars = []
colors = [(204, 204, 204),  # cinza claro
          (255, 204, 153),  # bege
          (204, 255, 204),  # verde claro
          (255, 153, 153),  # rosa claro
          (204, 204, 255),  # azul claro
          (255, 204, 204),  # rosa claro
          (153, 255, 255),  # ciano claro
          (255, 255, 153),  # amarelo claro
          (153, 204, 255),  # azul claro
          (255, 153, 255)]  # roxo claro

def escrever_arquivo(conteudo): #Essa função
    timeNow = time.time()
    with open('test.txt', 'w') as file:
        file.write(json.dumps(conteudo) + '\n')
        # for carro in conteudo.keys():
        #     adiciona_carro.delay(carro, "rodovia", timeNow, counteudo[carro][0], counteudo[carro][1])

print(time.time())

def processar_resultados():
    global cars, params, timer, to_save, total_time, numberSaved, saveTime

    ms = clock.tick(fps)
    to_save_frame = {}
    if interface_graph:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
    timerCreate -= ms
    if np.random.rand() < params['probabilidadeDeTrocaDeFaixa']:
        if len(cars) > 0:
            np.random.choice(cars).changeLane()
    if timerCreate <= 0:
        if np.random.rand() > params['probabilidadeDeEntradaDeVeiculo']:
            lane = np.random.randint(-params['sentido1Faixas'], params['sentido2Faixas'])
            pId = np.random.randint(0,len(plate))
            p = plate[pId]['placa']
            # remove p of plate dictionary
            plate.pop(pId)
            cars.append(carros.Cars(lane,Width,Height,100,colors[np.random.randint(0,len(colors))],p,params))
        timerCreate = timerCreate0
    to_save_frame = update(to_save_frame,ms)
    to_save[total_time] = to_save_frame
    saveTime += str(total_time) + '\n'
    to_save = json.dumps(to_save)
    
    thread_escrita = threading.Thread(target=escrever_arquivo, args=(path + key + "_" + str(numberSaved) + ".json", to_save))
    thread_escrita.start()
    
    to_save = {}
    
    # adiciona registro da ordem de leitura dos frames
    escrever_arquivo(pathdt + key + "_" + str(numberSaved) + ".txt", str(datetime.datetime.now()) + "\n" + saveTime)
    saveTime = ''
    numberSaved += 1
    total_time += ms
    if interface_graph:
        draw()

def draw():
    screen.fill(bg)
    br101.draw(screen, Width, Height)
    for car in cars:
        car.draw(screen)
    if interface_graph:
        pygame.display.update()

def update(to_save, ms):
    for car in cars:
        toDelete = car.update(ms, params['tempoColisao'])
        if toDelete:
            p = car.placa
            plate.append({'placa': p})
            cars.remove(car)
        p, pos = car.getData()
        to_save[p] = pos
        for car2 in cars:
            if car != car2:
                car.colision(car2, params['probabilidadeColisao'])
    return to_save

timerCreate0 = 100
timerCreate = timerCreate0
to_save = {}
total_time = 0
lock = threading.Lock()
numberSaved = 0

def processar_resultados():
    global cars, params, timer, to_save, total_time, numberSaved, saveTime,timerCreate

    while True:
        with lock:
            ms = clock.tick(fps)
            to_save_frame = {}
            if interface_graph:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        pygame.quit()
                        quit()
            timerCreate -= ms
            if np.random.rand() < params['probabilidadeDeTrocaDeFaixa']:
                if len(cars) > 0:
                    np.random.choice(cars).changeLane()
            if timerCreate <= 0:
                if np.random.rand() > params['probabilidadeDeEntradaDeVeiculo']:
                    lane = np.random.randint(-params['sentido1Faixas'], params['sentido2Faixas'])
                    pId = np.random.randint(0, len(plate))
                    p = plate[pId]['placa']
                    # remove p of plate dictionary
                    plate.pop(pId)
                    cars.append(carros.Cars(lane, Width, Height, 100, colors[np.random.randint(0, len(colors))], p, params))
                timerCreate = timerCreate0
            to_save = update(to_save_frame, ms)

        thread_escrita = threading.Thread(target=escrever_arquivo, args=(to_save,))
        thread_escrita.start()

        if interface_graph:
            draw()        


# Função principal
def main():
    thread_processamento = threading.Thread(target=processar_resultados)

    if interface_graph:
        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    quit()

    thread_processamento.start()
    thread_processamento.join()
    print("Processamento concluído.")

if __name__ == "__main__":
    main()


import pygame
import json
import pistas
import carros
import numpy as np

br101 = pistas.Road()

params = json.load(open('parametros.json'))

plate = json.loads(open('placas.json').read())

interface_graph = params['interfaceGrafica']

path = "../data/"
pathdt = '../delta-time/'

if interface_graph:
    pygame.init()

Width = 1280
Height = 720
if interface_graph:
    screen = pygame.display.set_mode((Width, Height))

if interface_graph:
    pygame.display.set_caption("Simulador de tráfego")

# Cor de fundo
if interface_graph:
    bg = (10, 130, 20)

# frames por segundo

fps = 60

# Relógio

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

cars.append(carros.Cars(np.random.randint(-2, 2),Width,Height,100,colors[np.random.randint(0,10)],plate[np.random.randint(0,10)]['placa']))

def draw():
    screen.fill(bg)
    br101.draw(screen, Width, Height)
    for car in cars:
        car.draw(screen)
    if interface_graph:
        pygame.display.update()

numberSaved = 0

timer = 1000

def update(timer,to_save,ms):
    isSaved = False
    for car in cars:
        toDelete = car.update(ms,params['tempoColisao'])
        if toDelete:
            p = car.placa
            plate.append({'placa':p})
            cars.remove(car)
        p, pos = car.getData()
        to_save_frame[p] = pos
        for car2 in cars:
            if car != car2:
                car.colision(car2,params['probabilidadeColisao'])
    if timer <= 0:
        isSaved = True
    return to_save,isSaved

timer = 5000
timerCreate0 = 100
timerCreate = timerCreate0
to_save = {}
total_time = 0
while True:
    ms = clock.tick(fps)
    to_save_frame = {}
    if interface_graph:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
    timer -= ms
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
            cars.append(carros.Cars(lane,Width,Height,100,colors[np.random.randint(0,10)],p))
        timerCreate = timerCreate0
    to_save_frame,isSaved = update(timer,to_save_frame,ms)
    to_save[total_time] = to_save_frame

    # adiciona registro da ordem de leitura dos frames
    with open(pathdt + params['nomeRodovia'] + "_" + str(numberSaved) + ".txt", 'a') as fl:
       fl.write(str(total_time)+'\n')
    
    if isSaved:
        print(to_save)
        to_save = json.dumps(to_save)
        # Serializing json
        # Writing to sample.json
        with open(path + params['nomeRodovia'] + "_" + str(numberSaved) + ".json", "w") as outfile:
            outfile.write(to_save)
        numberSaved += 1
        timer = 5000
        to_save = {}
    total_time += ms
    if interface_graph:
        draw()


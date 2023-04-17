import pygame
import json
import pistas
import carros
import numpy as np

br101 = pistas.Road()

params = json.load(open('parametros.json'))

plate = json.loads(open('placas.json').read())

path = "../data/"

pygame.init()

Width = 1280
Height = 720

screen = pygame.display.set_mode((Width, Height))

pygame.display.set_caption("Simulador de tráfego")

# Cor de fundo

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
    pygame.display.update()

numberSaved = 0

timer = 1000

def update(timer,to_save):
    isSaved = False
    for car in cars:
        toDelete = car.update()
        if toDelete:
            cars.remove(car)
        to_save.append(car.getData())
    if timer <= 0:
        isSaved = True
    return to_save,isSaved

timer = 5000
timerCreate = 300
to_save = {}
total_time = 0
while True:
    ms = clock.tick(fps)
    to_save_frame = []
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            quit()
    timer -= ms
    timerCreate -= ms
    if np.random.rand() < params['probabilidadeDeTrocaDeFaixa']:
        print("troca")
        if len(cars) > 0:
            np.random.choice(cars).changeLane()
    if timerCreate <= 0:
        if np.random.rand() > params['probabilidadeDeEntradaDeVeiculo']:
            lane = np.random.randint(-params['sentido1Faixas'], params['sentido2Faixas'])
            cars.append(carros.Cars(lane,Width,Height,100,colors[np.random.randint(0,10)],plate[np.random.randint(0,10)]['placa']))
        timerCreate = 1000
    to_save_frame,isSaved = update(timer,to_save_frame)
    to_save[total_time] = to_save_frame
    if isSaved:
        to_save = json.dumps(to_save)
        # Serializing json
        #Writing to sample.json
        with open(path + params['nomeRodovia'] + "_" + str(numberSaved) + ".json", "w") as outfile:
            outfile.write(to_save)
        numberSaved += 1
        timer = 5000
        to_save = {}
    total_time += ms
    draw()


import pygame
import json
import pistas
import carros
import numpy as np

br101 = pistas.Road()

params = json.load(open('parametros.json'))

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

#br101.setNumberLanes(params['sentido1Faixas'], params['sentido2Faixas'])

br101.setNumberLanes(2,2)

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

cars.append(carros.Cars(-1,Width,Height,100,colors[np.random.randint(0,10)]))

def draw():
    screen.fill(bg)
    br101.draw(screen, Width, Height)
    for car in cars:
        car.draw(screen)
    pygame.display.update()

def update():
    for car in cars:
        car.update()

while True:
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            quit()
    update()
    draw()
    clock.tick(fps)


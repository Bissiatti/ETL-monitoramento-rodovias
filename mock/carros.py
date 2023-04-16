import pygame
import json
import numpy as np

params = json.load(open('parametros.json'))

# class RoadConstants():
#     def __init__(self):


class Cars():
    def __init__(self,Lane,Width,Height,km0,color,placa):
        self.maxSpeed = params['velocidadeMaxima']
        self.minSpeed = params['velocidadeMinima']
        self.maxAcc = params['AceleracaoMaxima']
        self.minAcc = params['AceleracaoMinima']
        if Lane < 0:
            self.posX = Lane*90 + Width/2 + 15
            self.direction = 1
            self.posY = -50
        else:
            self.posX = Lane*90 + Width/2 + 15
            self.direction = -1
            self.posY = Height + 80
        self.lane = Lane
        self.vel = np.random.randint(params['velocidadeMinima'],params['velocidadeMaxima']+1)
        self.acc = 0
        self.dec = 0
        self.lengthx = 50
        self.lengthy = 75
        self.km0 = km0
        self.color = color
        self.crash = False
        self.placa = placa

    def draw(self,screen):
        pygame.draw.rect(screen, self.color, pygame.Rect(self.posX,self.posY,self.lengthx,self.lengthy))

    def update(self):
        print(self.vel)
        print(self.posX,self.posY)
        if self.vel > self.maxSpeed:
            self.vel = self.maxSpeed
            self.posY += self.vel*self.direction
            self.vel += self.acc*self.direction
            self.acc += 0.1*self.direction
        elif self.vel < self.minSpeed:
            if self.crash:
                self.posY += self.vel*self.direction
                self.vel -= self.dec*self.direction
            else:
                self.vel = self.minSpeed
                self.posY += self.vel*self.direction
                self.vel += self.acc*self.direction
                self.acc += 0.1*self.direction
        else:
            self.posY += self.vel*self.direction
            self.vel += self.acc*self.direction
            self.acc += 0.1*self.direction
        if self.acc > self.maxAcc:
            self.acc = self.maxAcc
        elif self.acc < self.minAcc:
            self.acc = self.minAcc

                

    def getData(self):
        return {self.placa:(self.posX*12**0.5-self.km0,self.posY)}
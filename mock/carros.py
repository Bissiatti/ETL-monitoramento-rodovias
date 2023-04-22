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
        self.Width = Width
        self.Height = Height
        if Lane < 0:
            self.posX = Lane*90 + Width/2 + 15
            self.direction = 1
            self.posY = -50
        else:
            self.posX = Lane*90 + Width/2 + 15
            self.direction = -1
            self.posY = Height + 50
        self.lane = Lane
        self.vel = np.random.randint(params['velocidadeMinima'],params['velocidadeMaxima']+1)
        self.acc = 0
        self.dec = 0
        self.lengthX = 50
        self.lengthY = 74
        self.km0 = km0
        self.color = color
        self.crash = False
        self.placa = placa
        self.x0 = self.posX
        self.y0 = self.posY
        self.trasintion = True
        self.timer = 0
        self.trasintionTime = 3000
        self.crash = False
        self.crashTimer = 0
        self.tryNotCrash = False

    def draw(self,screen):
        pygame.draw.rect(screen, self.color, pygame.Rect(self.posX,self.posY,self.lengthX,self.lengthY))

    def update(self,ms,timeColision):
        if not self.crash and not self.tryNotCrash:
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
            if self.trasintion:
                self.isTrasintion(ms)
            if (self.posY - self.y0)**2 > 100**2:
                self.trasintion = False
            # if distance is more than Width delete car
            if (self.posY - self.y0)**2 > 900**2:
                return True
            else:
                return False
        elif self.tryNotCrash:
            self.changeLane()
            self.dessacelerate(ms)
            self.trasintion = True
            self.tryNotCrash = False
            return False
        else:
            self.crashTimer += ms
            self.vel = 0
            self.acc = 0
            if self.crashTimer > timeColision:
                return True
            else:
                return False
    def changeLane(self):
        if not self.crash:
            if self.trasintion:
                if self.lane == params['sentido2Faixas'] - 1 or self.lane == - 1:
                    self.lane -= 1
                elif self.lane == - params['sentido1Faixas'] or self.lane == 0:
                    self.lane += 1
                else:
                    self.lane += (- 1) ** np.random.randint(2)
                if self.lane < 0:
                    self.posX = self.lane*90 + self.Width/2 + 15
                    self.direction = 1
                else:
                    self.posX = self.lane*90 + self.Width/2 + 15
                    self.direction = -1
        self.trasintion = True

    def isTrasintion(self,ms):
        if self.trasintion:
            self.timer += ms
            if self.timer < self.trasintionTime:
                self.trasintion = False
                self.timer = 0

    def colision(self,car2,prob):
        if (self.posY - self.y0)**2 > 120**2:
            if self.lane == car2.lane:
                if self.direction == 1:
                    if self.posY + self.lengthY > car2.posY and self.posY < car2.posY + car2.lengthY:
                        self.crash = True
                        car2.crash = True
                        self.tryNotCrash = False
                        return True
                else:
                    if self.posY < car2.posY + car2.lengthY and self.posY + self.lengthY > car2.posY:
                        self.crash = True
                        car2.crash = True
                        self.tryNotCrash = False
            if self.lane == car2.lane:
                if self.direction == 1:
                    if self.posY + 3*self.lengthY > car2.posY and np.random.rand() < prob:
                        car2.tryNotCrash = True
                    if self.posY < car2.posY + 3*car2.lengthY and np.random.rand() < prob:
                        self.tryNotCrash = True
                else:
                    if self.posY > car2.posY + 3*car2.lengthY and np.random.rand() < prob:
                        self.tryNotCrash = True
                    if self.posY + 3*self.lengthY < car2.posY and np.random.rand() < prob:
                        car2.tryNotCrash = True
        return False
    
    def dessacelerate(self,ms):
        if self.vel > 0:
            self.vel -= ms
        elif self.vel < 0:
            self.vel = 1

    def getData(self):
        return self.placa,(self.posX,self.posY*12**0.5+self.km0)
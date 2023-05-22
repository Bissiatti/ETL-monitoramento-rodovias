import pygame
class Road():
    def __init__(self):
        print("Road created")
        pass

    def setNumberLanes(self, numL, numR):
        print("Number of lanes set")
        self.numL = numL
        self.numR = numR
        self.length = 50

    def draw(self,screen, width, height):
        # draw a rectangle for each lane
        color = (160,160,150)
        yellowLine = (255,255,0)
        lanesLen = 90
        for i in range(self.numL):
            i = i+1
            pygame.draw.rect(screen, color, pygame.Rect(width/2-i*lanesLen-2*i,0,lanesLen,height))
        for i in range(self.numR):
            pygame.draw.rect(screen, color, pygame.Rect(width/2+i*lanesLen+2*i,0,lanesLen,height))
        # # draw a line in the middle
        pygame.draw.line(screen, yellowLine, (width/2,0), (width/2,height))
        pygame.draw.line(screen, yellowLine, (width/2+1,0), (width/2+1,height))
        pygame.draw.line(screen, yellowLine, (width/2-1,0), (width/2-1,height))
        pygame.draw.line(screen, yellowLine, (width/2-2,0), (width/2-2,height))



        
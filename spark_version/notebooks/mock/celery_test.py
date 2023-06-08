from tasks import adiciona_carro

if __name__ == '__main__':
    adiciona_carro.delay('ABC-1234', 'BR-101', 1.5, 10.2, 5.6)
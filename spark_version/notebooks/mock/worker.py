from celery import Celery

app = Celery('tasks', broker='pyamqp://guest@host.docker.internal:5672//')

if __name__ == '__main__':
    app.worker_main(['worker', '--loglevel=INFO'])
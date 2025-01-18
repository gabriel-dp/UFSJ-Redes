from locust import HttpUser, task

class Player(HttpUser):
    @task
    def game_state(self):
        self.client.get("/game")

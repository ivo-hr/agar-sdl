#include "agar-server.h"


//player

void handleCollisions() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].alive) {
            for (int j = 0; j < MAX_PLAYERS; j++) {
                if (players[j].alive && i != j) {
                    float disX = fabs(players[i].x - players[j].x);
                    float disY = fabs(players[i].y - players[j].y);
                    float radiusSum = players[i].radius + players[j].radius;

                    if ((disX <= radiusSum && disY <= radiusSum) || disX <= players[j].radius && disY <= players[i].radius) {
                        if (players[i].radius > players[j].radius && players[j].alive) {
                            players[j].alive = false;
                            reSpawnPlayer(j);
                            players[i].radius += players[j].radius / 2;
                        }
                    }
                }
            }
        }
    }
}

void reSpawnPlayer(int playerNum)
{

    players[playerNum].x = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
    players[playerNum].y = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
    players[playerNum].radius = INI_RADIUS;
    players[playerNum].alive = true;
}

void MovePlayer(int playerNum, int mouseX, int mouseY)
{
    float dx = (float)mouseX - WINDOW_WIDTH / 2;
    float dy = (float)mouseY - WINDOW_HEIGHT / 2;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > 0)
    {
        players[playerNum].x += dx * LAG_FACTOR;
        players[playerNum].y += dy * LAG_FACTOR;
    }
}

void initializePlayers()
{
    printf("jugadores inicializados: ");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].alive = false;
        players[i].x = 0;
        players[i].y = 0;
        players[i].radius = 0;
    }
}

void removePlayer(int playerNum)
{
    players[playerNum].x = 0.0;
    players[playerNum].y = 0.0;
    players[playerNum].radius = 0.0;
    players[playerNum].alive = false;
    players[playerNum].playerIndex = false;
    players[playerNum].playerIndex = -1;
}

//food

void CollisionFood()
{
    for (int i = 0; i < MAX_FOOD; i++)
    {
        if (foods[i].alive)
        {
            for(int j = 0; j < MAX_PLAYERS; j++)
            {
                float dx = foods[i].x - players[j].x;
                float dy = foods[i].y - players[j].y;
                float distance = sqrt(dx * dx + dy * dy);

                if (distance < (players[j].radius) + FOOD_RADIUS)
                {
                    foods[i].alive = false;
                    players[j].radius +=5;
                }
            }
        }
    }

}

void insertFood(Food food[], int x, int y)
{
    int i = 0;
    while (food[i].alive)
        i++;

    food[i].x = x;
    food[i].y = y;
    food[i].alive = true;
}

void generateFood(Food food[], int numFood)
{
    for (int i = 0; i < numFood; i++)
    {
        int x = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
        int y = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
        insertFood(food, x, y);
    }
}

void initializeFood()
{
    printf("comida inicializada: ");
    for (int i = 0; i < MAX_FOOD; i++)
    {
        foods[i].alive = false;
    }
}

void removeFood(Food food[], int index)
{
    food[index].alive = false;
}

//server/client

void do_messages()
{
    while (true)
    {

        Message em;

        Socket *client;

        mySocket->recv(em, &client);

        if(em.type == LOGIN)
        {
            std::unique_ptr<Socket> client_ptr(client);

            clients.push_back(std::move(client_ptr));
            //guardar la pos de clients usando numConnectedPlayers
            numConnectedPlayers++;
        }
        else if(em.type == LOGOUT)
        {

            auto it = clients.begin();

            while (it != clients.end())
            {
                if (*it->get() == *client)
                {
                    numConnectedPlayers--;
                    clients.erase(it);
                }

                ++it;
            }

        }
        else if(em.type == INPUT)
        {

            /*aun no se ja hecho el input, hay qie hacerlo*/

            //hacer el input
            auto it = clients.begin();

            InputMessage iMsg;
            iMsg.from_bin(em.data());

            while (it != clients.end())
            {
                if (*it->get() == *client)
                {

                    int position = std::distance(clients.begin(), it);

                    MovePlayer(position, iMsg.input.first, iMsg.input.second);
           
                    //players
                    std::vector<std::pair<float, float>> playerPos;
                    for(int i = 0; i < MAX_PLAYERS; i++)
                    {
                        std::pair<float, float> pos{players[i].x, players[i].y};
                        playerPos.push_back(pos);
                    }
                    PositionMessage pMsg(playerPos);
                    mySocket->send(pMsg, *(*it));
                    //sizes
                    std::vector<int> playerSize;
                    for(int i = 0; i < MAX_PLAYERS; i++)
                    {
                        int size = players[i].radius;
                        playerSize.push_back(size);
                    }
                    SizeMessage sMsg(playerSize);
                    mySocket->send(sMsg, *(*it));
                    //food
                    std::vector<std::pair<float, float>> foodPos;
                    for(int i = 0; i < MAX_FOOD; i++)
                    {
                        std::pair<float, float> pos{foods[i].x, foods[i].y};
                        if(!foods[i].alive)
                            foodPos.push_back(std::make_pair(-1, -1));
                        else foodPos.push_back(pos);
                    }
                    PositionMessage fMsg(foodPos);
                    mySocket->send(fMsg, *(*it));

                    //socket.send(em, *(*it));
                }
                ++it;
            }

        }
       

        handleCollisions();
        CollisionFood();
    }
}

int main() {

    char port[16];
    printf("Welcome to the AGAR.SDL server!\n\n To play this game truly online, you will need to port forward. If you don't, you will be restricted to your network. \n Info about how you can port forward your router can be found here: https://portforward.com/how-to-port-forward/\n\n");
    sleep(2);
    printf("Enter the port number: ");
    fgets(port, sizeof(port), stdin);

    char IP[16];
    printf("Ingrese la ip del servidor: ");
    fgets(IP, sizeof(IP), stdin);
    IP[strcspn(IP, "\n")] = '\0';

    port[strcspn(port, "\n")] = '\0';
    IP[strcspn(IP, "\n")] = '\0';

    printf("ip coppied succesfull\n");
    Socket* serverSocket = new Socket(IP, port);
    if (serverSocket->bind() == -1) {
        perror("Error al crear el socket del servidor");
        return 1;
    }
    mySocket = serverSocket;
    printf("Socket del servidor creado exitosamente\n");

    initializeFood();
    initializePlayers();
    do_messages();

    printf("servidor cerrado: ");

    return 0;
}

//do_message
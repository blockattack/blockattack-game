/*
NetworkThing.hpp
Copyright (C) 2007 Poul Sander

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Poul Sander
    R�veh�jvej 36, V. 1111
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com
*/

#if NETWORK

//The network interface
class NetworkThing
{
private:
    ENetAddress address;
    ENetHost * server;
    ENetHost * client;
    ENetPeer *peer;

    BlockGame *bgHome, *bgAway; //Pointers to the two games, so we can call the procedures
    bool weAreAServer, weAreAClient;
    bool weAreConnected;
    bool enemyIsReady;
    bool enemyHasStarted; //If we should have a ready button
    bool gameHasStarted; //If the player is GameOver it might just be because the game hasn't started yet... not if this is true!

public:

    Uint32 theSeed;

    NetworkThing()
    {
        weAreAServer = false;
        weAreAClient = false;
        weAreConnected = false;
        enemyIsReady = false;
        enemyHasStarted = false;
        gameHasStarted = false;
        if (enet_initialize () != 0)
        {
            fprintf (stderr, "An error occurred while initializing ENet.\n");
        }
        else
        {
            cout << "Network is working!" << endl;
        }
        //atexit (enet_deinitialize); in deconstructor
    }

    //This function couses us to remove the connection to the other peer, plus destroying servers
    void ntDisconnect()
    {
        strcpy(bgAway->name,player2name);
        if (weAreConnected || weAreAClient || weAreAServer)
        {
            if (weAreAClient)
            {
                enet_peer_disconnect (peer,0);
                SDL_Delay(20);
                enet_host_destroy(client);
                weAreAClient = false;
                weAreConnected = false;
                enemyHasStarted = false;
            }
            if (weAreAServer)
            {
                enet_host_destroy(server);
                weAreAServer = false;
                weAreConnected = false;
                enemyHasStarted = false;
            }
            //If the game is running then we disconnect, we are considered the looser!
            if ((!bgHome->isGameOver())&&(!bgAway->isGameOver()))
            {
                bgAway->setPlayerWon();
                bgHome->SetGameOver();
            }
            gameHasStarted = false;
        }
    };

    //Lets disconnect before we close...
    ~NetworkThing()
    {
        cout << "Network system is going down" << endl;
        ntDisconnect();
        enet_deinitialize(); 
        cout << "Network system went down" << endl;
    }

    void theGameHasStarted()
    {
        gameHasStarted = true;
    }

    //The NetworkThing needs to be able to call the models... here the pointers are set
    void setBGpointers(BlockGame *bg1, BlockGame *bg2)
    {
        bgHome = bg1;
        bgAway = bg2;
    }

    //Starts a server instance
    void startServer()
    {
        if (!weAreAServer)
        {
            ntDisconnect();

            /* Bind the server to the default localhost.     */
            /* A specific host address can be specified by   */
            /* enet_address_set_host (& address, "x.x.x.x"); */

            address.host = ENET_HOST_ANY;
            /* Bind the server to port SERVERPORT. */
            address.port = SERVERPORT;

            server = enet_host_create (& address /* the address to bind the server host to */,
                                       1      /* allow up to 1 clients and/or outgoing connections */,
                                       4      /* allow 4 channels to be used */,
                                       0      /* assume any amount of incoming bandwidth */,
                                       0      /* assume any amount of outgoing bandwidth */);
            if (server == NULL)
            {
                fprintf (stderr,
                         "An error occurred while trying to create an ENet server host.\n");

            }
            else
            {
                weAreAServer = true;
                enemyIsReady = false;
                gameHasStarted = false;
                cout << "Server is listening on port " << SERVERPORT << endl;
            }
        }
    }

    void connectToServer(string server)
    {
        ENetAddress address;
        ENetEvent event;

        enet_address_set_host (& address, server.c_str());
        address.port = SERVERPORT;

        ntDisconnect();
        client = enet_host_create (NULL /* create a client host */,
                                   1 /* only allow 1 outgoing connection */,
                                   4 /* allow 4 channels to be used */,
                                   0 /* Unlimited downstream bandwidth */,
                                   0 /* Unlimted upstream bandwidth */);

        if (client == NULL)
        {
            cout << "An error occurred while trying to create an ENet client host." << endl;
        }
        else
        {
            /* Initiate the connection, allocating the four channels 0 and 1 and 2 and 3. */
            peer = enet_host_connect (client, & address, 4,0);

            if (peer == NULL)
            {
                cout << "No available peers for initiating an ENet connection." << endl;
            }
            else
                if (enet_host_service (client, & event, 2000) > 0 &&
                        event.type == ENET_EVENT_TYPE_CONNECT)
                {
                    cout << "We are connected!" << endl;
                    enemyIsReady = false;
                    weAreAClient = true;
                    weAreConnected = true;
                    gameHasStarted = false;
                }
                else
                {
                    cout << "Server didn't answer in time" << endl;
                }
        }
    }

    bool isConnected()
    {
        return (weAreConnected||weAreAServer);
    }

    bool isConnectedToPeer()
    {
        return weAreConnected;
    }

    //This function must be called in the game loop:
    void updateNetwork()
    {
        //Now we must send our own board:
        if (weAreConnected)
        {
            //cout << "Creating package" << endl;
            boardPackage boardpack = bgHome->getPackage();
            ENetPacket * packet = enet_packet_create (&boardpack,
                                  sizeof(boardPackage),
                                  0);
            //Now lets send the package
            if (weAreAServer)
                enet_host_broadcast (server, 0, packet);
            else
                if (weAreAClient)
                    enet_peer_send (peer, 0, packet);
            //cout << "Package sent" << endl;

            //See if we are game over and in that case notify the other player
            if ((gameHasStarted)&&(bgHome->isGameOver()))
            {
                ENetPacket * packet = enet_packet_create("G",2,ENET_PACKET_FLAG_RELIABLE);
                if (weAreAServer)
                    enet_host_broadcast (server, 1, packet);
                else
                    if (weAreAClient)
                        enet_peer_send (peer, 1, packet);
                gameHasStarted=false;
            }

            //Now lets see if we have something to throw at the opponent.
            Uint8 x,y,type;

            if (gameHasStarted)
                while (bgAway->popGarbage(&x,&y,&type))
                { //if there are garbage to drop
                    if (type==1)
                    {
                        x=255;
                        y=255;
                    }
                    Uint16 g = ((Uint16)x)*256+(Uint16)y; //A 16-bit int containing everything
                    ENetPacket * packet = enet_packet_create(&g,2,ENET_PACKET_FLAG_RELIABLE);
                    if (weAreAServer)
                        enet_host_broadcast (server, 2, packet);
                    else
                        if (weAreAClient)
                            enet_peer_send (peer, 2, packet);
                    cout << "We send a garbage block: " << (int)x << "," << (int)y << "==" << g << endl;
                }
        }
        ENetEvent event;

        /* Wait up to 0 milliseconds for an event. */
        while (((weAreAClient)&&(enet_host_service (client, & event, 0) > 0))||((weAreAServer)&&(enet_host_service (server, & event, 0) > 0)))
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                printf ("A new client connected from %x:%u.\n",
                        event.peer -> address.host,
                        event.peer -> address.port);
                weAreConnected = true;
                /* Store any relevant client information here. */
                //event.peer -> data = "Client";
                {

                    ENetPacket * namePacket = enet_packet_create(bgHome->name,sizeof(char[30]),ENET_PACKET_FLAG_RELIABLE);
                    //if(weAreAServer)
                    enet_host_broadcast (server, 3, namePacket);
                    ENetPacket * answerPacket = enet_packet_create("version3",sizeof("version3"),ENET_PACKET_FLAG_RELIABLE);
                    enet_host_broadcast (server, 3, answerPacket);
                    theSeed = time(0)/4;
                    bgHome->putStartBlocks(theSeed);
                    ENetPacket * timePacket = enet_packet_create(&theSeed,sizeof(theSeed),ENET_PACKET_FLAG_RELIABLE);
                    enet_host_broadcast (server, 3, timePacket);
                    cout << "We send the seed: " << theSeed << endl;
                }
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                /*printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                    event.packet -> dataLength,
                    event.packet -> data,
                    event.peer -> data,
                    event.channelID);*/
                //cout << "Package recieved" << endl;
                if (event.channelID==0) //Unreliable (only boardPacks)
                {
                    boardPackage bpack;
                    //cout << "Package size: "<< event.packet->dataLength << " should be: " << sizeof(boardPackage) << endl;
                    memcpy(&bpack,(const char*)event.packet->data,sizeof(boardPackage));
                    bgAway->setBoard(bpack);
                }
                if (event.channelID==1) //reliable (used for GameOver notifications only!)
                {
                    if ((bgHome->isGameOver())&&(!bgHome->GetIsWinner())&&(gameHasStarted))
                    {
                        bgHome->setDraw();
                        bgAway->setDraw();
                    }
                    else
                    {
                        bgHome->setPlayerWon();
                        bgAway->SetGameOver();
                    }
                    gameHasStarted=false;
                }

                if (event.channelID==2) //reliable (used for Garbage only!)
                {
                    Uint16 g;
                    memcpy(&g,event.packet->data,sizeof(Uint16));
                    Uint8 x = (g/256);
                    Uint8 y = (g%256);
                    cout << "Recieved Garbage: " << (int)x << "," << (int)y << endl;
                    if ((x==255)&&(y==255))
                        bgHome->CreateGreyGarbage();
                    else
                        bgHome->CreateGarbage(x,y);
                }

                if (event.channelID==3) //We have reacieved a name or a version number
                {
                    if (event.packet->dataLength==sizeof(char[30])) //We have reveived a name
                    {
                        strcpy(bgAway->name,(const char*)event.packet->data);
                        cout << "The enemy name is: " << bgAway->name << " Length: " << sizeof(char[30])<< endl;
                        if (weAreAClient) //We have just recieved the servers name and must send our own
                        {
                            ENetPacket * answerPacket = enet_packet_create(bgHome->name,sizeof(char[30]),ENET_PACKET_FLAG_RELIABLE);
                            enet_peer_send (peer, 3, answerPacket);
                        }
                    }
                    else
                        if (event.packet->dataLength==sizeof("version3")) //We have recieved aversion number
                        {
                            if (0!=strcmp((const char*)event.packet->data,"version3"))
                            {
                                cout << "Incompatible version: " << event.packet->data << "!=" << "version3" << endl;
                                ntDisconnect();
                            }
                            if (weAreAClient) //We will send our version number
                            {
                                ENetPacket * answerPacket = enet_packet_create("version3",sizeof("version3"),ENET_PACKET_FLAG_RELIABLE);
                                enet_peer_send (peer, 3, answerPacket);
                            }
                        }
                        else
                            if (event.packet->dataLength==sizeof(Uint32)) //We have recieved a seed
                            {
                                memcpy(&theSeed,event.packet->data,sizeof(Uint32));
                                bgHome->putStartBlocks(theSeed);
                                cout << "We recieved a seed: " << theSeed << endl;
                            }
                }

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);

                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                //printf ("%s disconected.\n", event.peer -> data);
                cout << event.peer -> data << " disconnected." << endl;

                /* Reset the peer's client information. */

                event.peer -> data = NULL;
                //weAreConnected = false;
                if ((!bgHome->isGameOver())&&(!bgAway->isGameOver()))
                {
                    bgHome->setPlayerWon();
                    bgAway->SetGameOver();
                }

                ntDisconnect(); //When we will disconnect!

            }
        }


    }
}; //NetworkThing

#endif

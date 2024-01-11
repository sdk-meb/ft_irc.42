# ft_irc 🌐

## Ft_irc Summary

1337's project to build a custom IRC server. IRC stands for Internet Relay Chat, and is one of the first chat protocol on the internet, made around 1988. Twitch still uses an IRC chat nowadays, and a network of servers still hosts thousands of channels.

## Mandatory Instructions

- You have to develop an IRC server in C++ 98.
- You mustn’t develop a client.
- You mustn’t handle server-to-server communication.
- Your executable will be run as follows:
  `./ircserv <port> <password>` - port: The port number on which your IRC server will be listening to for incoming
  IRC connections. - password: The connection password. It will be needed by any IRC client that tries
  to connect to your server.
- The server must be capable of handling multiple clients at the same time and never
  hang.
- Forking is not allowed. All I/O operations must be non-blocking.
- Only 1 poll() (or equivalent) can be used for handling all these operations (read,
  write, but also listen, and so forth).
- Several IRC clients exist. You have to choose one of them as a reference. We chose _irssi_.
- Your reference client must be able to connect to your server without encountering
  any error.
- Communication between client and server has to be done via TCP/IP (v4 or v6).
- Using your reference client with your server must be similar to using it with any
  official IRC server. However, you only have to implement the following features: - You must be able to authenticate, set a nickname, a username, join a channel,
  send and receive private messages using your reference client. - All the messages sent from one client to a channel have to be forwarded to
  every other client that joined the channel. - You must have operators and regular users. - Then, you have to implement the commands that are specific to operators.

# Installation instructions

Clone the project and launch `make` to build the server executable 

### Launch the server

```

./ircserv [port number] [password]

```

## Connect to the server with a Netcat

```

nc -C 127.0.0.1 6667

```

**note** : _we use the -C option because IRC messages end with **\r\n** as defined in the RFC 2812 standards._

**note** : _We used 127.0.0.1 since our server is launched in localhost._

## Authenticate

You need to authenticate to the server otherwise you will be deconnected quickly :

```sh
pass anyPass
user '<ops>' 0 * :'<ops>'
nick '<ops>'
```

**note** : _To understand more abour the authentification process please look [here](https://www.rfc-editor.org/rfc/rfc2812#section-3.1.1)._

Enjoy!

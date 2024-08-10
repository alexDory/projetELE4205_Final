# Projet ELE4205-28 - Système Client-Serveur de Transmission Vidéo avec Détection de Visages

Bienvenue dans le projet ELE4205-28 ! Ce projet est une implémentation d'un système client-serveur utilisant le protocole TCP/IP pour la transmission vidéo en temps réel avec détection de visages. L'équipe de développement est composée d'Alex Doury et Alexandre Auffray.

## Description

Le système se compose de deux parties principales : le client et le serveur.

- **Client (CentOS 7):** Le client tourne sur une machine CentOS 7. Il est responsable de recevoir le flux vidéo en temps réel du serveur et de l'afficher. Lorsqu'un bouton associé à l'Odroid est pressé, le client recoit une image unique, la transmet au serveur et effectue une analyse de détection de visages à l'aide d'OpenCV.

- **Serveur (Odroid C2):** Le serveur s'exécute sur un Odroid-C2 et est connecté à une caméra Logitech. Il transmet un flux vidéo continu au client. Lorsqu'un bouton est pressé, le serveur capture une image unique et la transmet au client.

## Fonctionnalités

- Transmission vidéo en temps réel du serveur au client.
- Capture d'une image unique sur pression d'un bouton sur l'Odroid.
- Analyse de la capture pour détecter et encadrer les visages à l'aide d'OpenCV.
- Vérification de la présence de la personne dans la base de données.
- Affichage du nom de la personne dans une pop-up avec l'image capturée.

## Configuration du Projet

1. Cloner le projet :

- git clone BITBUCKET

2.Lancer deux terminaux
	— un en bash avec compilation croisée
- cd serveur
- bash
- source /usr/local/opt/poky/2.1.3/environment-setup-aarch64-poky-linux
- code --extensions-dir /export/tmp/${USER}/vscode-ext .

	— un en tcsh avec compilation en natif
-  cd client
-  code --extensions-dir /export/tmp/${USER}/vscode-ext
 
3.Sur l'odroid executer :

-  modprobe pwm-meson
-  modprobe pwm-ctrl
-  echo 228 > /sys/class/gpio/export
-  echo in > /sys/class/gpio/gpio228/direction
-  echo 512 > duty0
-  echo 440 > freq0 


4.Executer les deux programmes , le serveur en premier puis le client.
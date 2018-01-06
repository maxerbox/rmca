# RMCA: ROBOT MONTE SUR CHENILLES AUTOMATISE

<img src="https://i.imgur.com/cFpBxkk.jpg" style="width: 250px"></img>


## Sommaire

- [RMCA: ROBOT MONTE SUR CHENILLES AUTOMATISE](#rmca-robot-monte-sur-chenilles-automatise)
    - [Sommaire](#sommaire)
    - [Introduction](#introduction)
    - [Environnement necessaire](#environnement-necessaire)
    - [Etudes réalisées](#etudes-r%C3%A9alis%C3%A9es)
    - [Structure de la repo git](#structure-de-la-repo-git)
        - [RMCA Manager](#rmca-manager)
        - [Code Arduino](#code-arduino)
        - [dossier mini projet_v1.docx](#dossier-mini-projetv1docx)

## Introduction

Le RMCA a été réalisé pour le Mini-Projet 2017.

Dans le cadre d'une introduction au projet annuel au BAC des STI2D SIN, le Mini-Projet a duré 2 mois.

4 Sujet on été proposé, dont celui de réaliser un robot qui remplirait ces taches:

- Etre controllé à distance (Partie télécommande)
- Mesurer la température ambiante et la transmettre à l'odinateur par Bluetooth (LE RMCA-Manager)
- Détecter les obstacles (Capteur à Ultrason)

## Environnement necessaire

Ce projet à été réalisé avec l'environnement Arduino, notament la carte Arduino UNO.
Voici les differentes parties qui ont été commandé sur le site **GOTRONIC**

| Reference | Nom Article                          | Quantité  | Prix TTC   | Montant Total |
|-----------|--------------------------------------|-----------|------------|---------------|
| 25950     | Carte Arduino Uno                    | 2         |   19,50 €  |   39,00 €     |
| 31297     | Module joystick Grove                | 1         |   8,50 €   |   8,50 €      |
| 31315     | Télémètre à ultrasons Grove          | 1         |   15,70 €  |   15,70 €     |
| 25483     | Capteur de température               | 1         |   15,90 €  |   15,90 €     |
| 31060     | Arduino Motor Shield 2 x 2 A         | 1         |   24,50 €  |   24,50 €     |
| 32838     | Motoréducteur HP 2361                | 2         |   17,60 €  |   35,20 €     |
| 05294     | Kit émetteur-récepteur 433MHZ        | 1         |   6,50 €   |   6,50 €      |
| 34070     | Dongle Bluetooth et Wifi USB ADA2649 | 1         |   20,95 €  |   20,95 €     |
| 33974     | Module Bluetooth Grove               | 1         |   19,60 €  |   19,60 €     |
| 32700     | Châssis Zumo à chenilles 1418        | 1         |   21,95 €  |   21,95 €     |
| TOTAL     |                                      |           |            |   207,80 €    |

## Etudes réalisées

Différentes étude avec des diagrammes SYSML ont été réalisée (voir dossier), afin de prouver toutes les mesures, nous avons relevées les trames de communication, etc...

## Structure de la repo git

### RMCA Manager

Le code source du RMCA Manager (logiciel permettant d'afficher les mesures) est présent dans le dossier RMCA Manager

![Presentation](https://i.imgur.com/zcDgErZ.gif)

### Code Arduino

Tout le code Arduino (partie télécommande et robot) est présent dans ce dossier. L'installation des librairies sont necessaires pour compiler

### dossier mini projet_v1.docx

Dossier rendu au proffesseur
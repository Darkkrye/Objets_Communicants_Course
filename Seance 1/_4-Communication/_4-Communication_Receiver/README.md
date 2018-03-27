_4-Communication_Receiver
=====

# Contenu
Le code permettant la réception des informations.

On sait que le **Sender** envoie une chaîne de 10 caractères. On boucle, donc, 10 fois, sur la fonction `read` de l'interface **Serial1** pour récupérer l'information depuis les pins et une fois arrivée à 10, on utilise l'interface **Serial** pour afficher l'information dans le Moniteur d'activité sur l'ordinateur.

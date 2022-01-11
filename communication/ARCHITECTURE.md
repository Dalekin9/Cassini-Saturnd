# Architecture :

## Fichiers :

### saturnd

Programme prinicipal de saturnd :
- Lance les tâches toutes les minutes.
- Lit l'opération sur le tube de requête.
- Lit le reste de la requête grâce à `read_request`.
- Ecrit la réponse grâce à `write_reply`.


### cassini

Programme prinicipal de cassini :
Selon l'opération voulue : 
- Ecrit la requête sur le tube de requête grâce à `write_request`.
- Lit la réponse grâce à `read_reply`.
- Ecrit la réponse grâce à `print_reply`.


### write-request

**Cassini**: Ecrit la requête sur le tube de requête.

### write-reply

**Saturnd**: Ecrit la réponse sur le tube de réponse.


### read-reply

**Cassini**: Lit la réponse sur le tube de réponse.


### read-request

**Saturnd**: Lit la requête sur le tube de requête.


### print-reply

**Cassini**: Ecrit la réponse sur stdout/stderr.


### common-read

Fonctions communes aux read :
- Lit un timing.
- Lit les arguments.
- Lit les tâches.
- Lit l'id des tâches.


### common-folder

Fonctions communes aux dossiers :
- Retourne les chemins des dossiers/fichiers principaux.


### create-task

Fonctions servant à créer une tâche :
- Créer un dossier pour la tâche.
- Créer les fichiers timing et argv (qui contiennent respectivement le timing et la commande de la tâche), et le fichier runs.


### folder

Créer les dossiers intermédiaires : (tmp, user, saturnd, pipes et tasks).


### pipes

S'occupe des pipes :
- Créer les pipes si nécessaire.
- Retourne les descripteurs des pipes selon l'ouverture voulue.


### run_tasks

Lance les tâches :
- Pour chaque tâche,  regarde si elle n'est pas supprimée et si le timing est correct.
- Lance la commande (à récupérer dans argv).


### timing-text-io

Fonctions liées aux conversions d'un timing


## Structures :

### task

 Structure d'une tâche utilisée dans `cassini`


### s_task

Structure d'une tâche utilisée dans `saturnd`


### run

Les données d'un lancement de la tâche associée.


### commandline

La longueur et les arguments de la ligne de commande.


### string

Une chaîne de caractères et sa longueur.


### BYTE

Entier non signé représentant un byte.


### timing

Jour, Heure, et Minute à laquelle doit être lancée une tâche.


## Misc
Suite à un problème de merge sur la branche 16, nous avons dû en créer une nouvelle ayant certaines modifications voulues.
Les commits de la branche 16 sont donc "perdus".



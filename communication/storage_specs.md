# Ce fichier decrit comment saturnd stocke les donnees sur le disque

Toutes les valeurs d'entiers sont dans le format uint_n ou int_n, en convention big endian.
Tout est dans le dossier "/tmp/<USERNAME>/saturnd".

## Numero taskid disponible
Dans ce dossier, il y a un fichier nomme "last_taskid" qui contient un
uint_64 => le dernier numero de tache qui a ete utilise (le prochain libre 
est donc last_taskid+1).

## pipes
Les pipes sont dans le sous-dossier "pipes" (a creer si il n'existe pas).

## taches
Creer un dossier "tasks" au demarrage si il n'existe pas.
Pour chaque tache, creer un dossier dont le nom est "<TASKID>" et qui contient 5 fichiers :
- un fichier "timing" qui contient les 3 champs de la structure
- un fichier "argv" qui contient la commandline (sous forme d'une seule chaine de char)
- deux fichiers "stdout" et "stderr" qui contiennent les resultats des derniers runs.
- un fichier "runs" qui contient la date et la valeur de retour de chaque execution
  On mettra toutes les executions dans ce meme fichier :
  - pour ecrire, on ajoute a la fin du fichier la derniere execution
  - pour lire, on lit a chaque fois un int64 (la date) et un uint16 (la val de retour),
    jusqu'a ce que le fichier soit vide

Quand on supprime une tache, on supprime le dossier correspondant.


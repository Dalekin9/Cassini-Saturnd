# Ce fichier décrit comment saturnd stocke les données sur le disque
Toutes les valeurs d'entiers sont dans le format uint_n ou int_n, en convention big endian.
Tout est dans le dossier "/tmp/<USERNAME>/saturnd".

## Numéro taskid disponible
Dans ce dossier, il y a un fichier nommé `last_taskid` qui contient un
uint_64 => le dernier numero de tache qui a été utilisé (le prochain libre 
est donc last_taskid+1).

## Pipes
Les pipes sont dans le sous-dossier `pipes` (à créer si il n'existe pas).

## Tâches
Créer un dossier "tasks" au demarrage si il n'existe pas.
Pour chaque tache, creer un dossier dont le nom est "<TASKID>" et qui contient 6 fichiers :

- Un fichier `timing` qui contient les 3 champs de la structure.
- Un fichier `argv` qui contient la commandline (même format de données que dans la requête).
- Deux fichiers `stdout` et `stderr` qui contiennent les résultats des derniers runs.
- Un fichier `nb_runs` qui contient le nombre de fois que la tâche correspondante a run.
- Un fichier `runs` qui contient la date et la valeur de retour de chaque exécution.
  On mettra toutes les exécutions dans ce même fichier :
  - Pour écrire, on ajoute à la fin du fichier la dernière exécution.
  - Pour lire, on lit à chaque fois un int64 (la date) puis un uint16 (la valeur de retour),
    jusqu'à ce que le fichier soit vide.

- Si la tâche a été supprimée un fichier `removed` vide sera créé.




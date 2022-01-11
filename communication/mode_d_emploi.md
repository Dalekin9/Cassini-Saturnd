# Mode d'emploi :

## Compilation : 

`make clean` : Supprime tous les fichiers ajoutés à la compilation.

`make` : compile tous les fichiers nécessaires à l'exécution.


## Exécution :

* Saturnd :

    ./saturnd


* Cassini :

    ./cassini avec :

    - l -> Liste toutes les tâches non supprimées.
    - q -> Termine le démon.
    - c [-m MINUTES] [-H HEURES] [-d DAYSOFWEEK] COMMAND_NAME [ARG_1] ... [ARG_N]\n\
            -> Crée une nouvelle tâche.
    - r `TASKID` -> Supprime la tâche.
    - x `TASKID` -> Retourne les infos (time + exit code) sur les derniers lancements de la tâche.
    - o `TASKID` -> Retourne la dernière sortie standard de la tâche.
    - e `TASKID` -> Retourne la dernière sortie erreur de la tâche.
    - h -> Affiche l'aide.
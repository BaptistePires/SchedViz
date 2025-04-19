# SchedViz

Ce dépot contient les modules, scripts pour pouvoir analyser les temps de blocages des threads dans Linux v5.17.
Ces travaux ont été réalisé dans le cadre de la thèse de Baptiste Pires.

# `module/`
Ce répertoire contient le code du module noyau Linux à insérer qui va s'occuper de réaliser les mesures et de les stocker. Il faut un noyau Linux v5.17 patché, se trouvant à l'adresse suivante : `todo: host `.

# `user/`
Ce répertoire contient tout le code userspace qui permet de lancer, mesurer, analyser et produire des graphiques.

Le script `monitor_cmd.sh` permet de monitorer une commande

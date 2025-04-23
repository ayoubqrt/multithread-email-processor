L'objectif de l'exercice était de découvrir le multithreading en C++. 
J'en ai profité pour utiliser les dernières fonctionnalités de C++17, afin d'écrire du code aussi moderne et optimisé que possible. (fonctions lambda, std::list, std::vector, std::thread, move, find)

Résumé fait par ChatGPT car je me souvenais pas exactement ce que faisait cet exercice
---

# **Résumé**

### 🎯 **Objectif du programme**
Analyser un ensemble d’emails (1 fichier = 1 email) pour déterminer, **par expéditeur**, **combien de mails** ont été envoyés **à chaque destinataire**, en utilisant le **multithreading** pour améliorer les performances du programme.

---

### 📝 **Contenu du fichier final**
- Chaque ligne correspond à un **expéditeur**.
- Pour chaque expéditeur : liste des **destinataires** et le **nombre de mails** envoyés à chacun.

**Format type** :
```
expediteur.txt: nb1:destinataire1 nb2:destinataire2 ...
```


### 1. **Fonctionement général du programme**
- Lire une base d'**emails** dans un dossier (1 fichier = 1 email).
- **Paralléliser** le parsing de ces emails avec plusieurs **threads**.
- **Enregistrer** les résultats intermédiaires dans `threads/`.
- **Fusionner** tous les résultats pour produire un fichier final `results.txt`.

---

### 2. **Détail du fonctionnement**

#### ➔ Phase 1 : Préparation
- Crée un dossier `threads/` pour stocker les résultats par thread.

#### ➔ Phase 2 : Répartition des emails
- Cherche tous les fichiers dans `./maildir`.
- Découpe la liste en **parts égales** selon le nombre de **threads CPU** disponibles (`hardware_concurrency()`).

#### ➔ Phase 3 : Lancement des Workers
- Chaque `Worker` reçoit une liste de fichiers.
- Chaque `Worker` est lancé pour **analyser** sa partie (classe `Worker` définie ailleurs).

#### ➔ Phase 4 : Fusion des résultats (`parseThreadsResults`)
- Lit les fichiers produits par chaque thread dans `threads/`.
- **Trie** les fichiers par **nom d’expéditeur**.
- **Regroupe** les résultats : pour chaque expéditeur, compte combien de fois il a envoyé un mail à chaque destinataire.
- Écrit tout dans `results.txt`.





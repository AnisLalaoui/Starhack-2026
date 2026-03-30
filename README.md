# MedBot – Compagnon intelligent de médication

Un compagnon de chevet intelligent qui rappelle, guide et automatise la prise de médicaments grâce à un système embarqué.

---


## Aperçu

MedBot est un système basé sur Arduino conçu pour réduire les oublis et les erreurs de prise de médicaments. Il combine rappels programmés, alertes progressives et distribution automatique pour accompagner l’utilisateur étape par étape.

---

## Problème

- Oubli de prise de médicaments  
- Mauvais timing ou mauvaise dose  
- Manque de suivi, surtout chez les personnes âgées ou occupées  

---

## Solution

MedBot propose :

-  **Rappels précis** grâce à une horloge temps réel  
-  **Alertes progressives** (douce → moyenne → urgente)  
-  **Distribution automatique des pilules**  
-  **Messages contextuels intelligents** (inspirés de l’IA)  
-  **Confirmation utilisateur via clavier**  

---

## Technologies utilisées

- **Arduino Uno R3**  
- **RTC DS1307 (horloge)**  
- **LCD 1602 (affichage)**  
- **Servo moteur (trappe)**  
- **Moteur pas à pas + ULN2003 (distribution)**  
- **Clavier (entrée utilisateur)**  
- **Buzzer + LED (alertes)**  
- **C++ embarqué (Arduino)**  

---

## Fonctionnalités

- ⏰ Plusieurs rappels par jour  
- 🔁 Rappel quotidien de remplissage  
- 🔊 Escalade intelligente des alertes  
- 💬 Messages dynamiques (santé / calendrier)  
- ⚙️ Intégration complète hardware + software  

---

##  Innovation

Le système simule une interaction intelligente en utilisant des messages contextuels et une logique adaptative directement embarquée, sans connexion internet ni cloud.

---

##  Architecture matérielle

| Composant | Broches |
|----------|--------|
| LCD 1602 | 2, 12, 13, A0, A1, A2 |
| RTC DS1307 | A4 (SDA), A5 (SCL) |
| Servo | 7 |
| Stepper (ULN2003) | 8, 9, 10, 11 |
| Buzzer | 3 |
| LED | A3 |
| Clavier | 4, 5, 6 |

---

##  Fonctionnement

1. L’horloge (RTC) suit l’heure réelle  
2. À une heure définie, un rappel est déclenché  
3. Les alertes deviennent progressivement plus fortes  
4. Une trappe s’ouvre et les pilules sont distribuées  
5. L’utilisateur confirme la prise  
6. Le système se réinitialise pour le prochain rappel  

---

##  Démonstration

*(Ajoutez ici le lien de la vidéo)*

---

##  Aperçu du prototype

*(Ajoutez ici des images du montage)*

---

## Améliorations futures

- Intégration d’une vraie IA (voix, NLP)  
- Application mobile  
- Suivi de santé et analytics  
- Connectivité IoT  

---


## ⚠️ Avertissement

Ce projet est un prototype et ne constitue pas un dispositif médical certifié.

---

## 🏁 Conclusion

MedBot démontre comment un système embarqué peut améliorer le suivi médical quotidien en combinant automatisation, interaction utilisateur et feedback intelligent.

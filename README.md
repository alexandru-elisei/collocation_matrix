Structuri de Date - TEMA 3
ELISEI ALEXANDRU
316CC

		GRAFURI DE CUVINTE

Cuprins:
	1. COMPILARE
	2. TASK BONUS
	3. DESCRIERE
	4. DETALII IMPLEMENTARE
	5. BONUS


1. COMPILARE

Programul este compilat cu comanda "make build (sau "make" simplu). La compilare
se poate modifica variabila CFLAGS ("make CFLAGS='-DDEBUG=1 -g'") si astfel pot
fi folosite macro-urile de debugging definite in common.h (DEBINFO si DEBMSG).

Fisierele rezultate sunt sterse cu comanda "make clean".


2. TASK BONUS

Am implementat si aflarea drumurilor de cost minim de lungime fixa cand se stie
nodul terminal.


3. DESCRIERE

Programul parseaza un text si pentru fiecare cuvant distinct si vecinii sai 
(cuvintul imediat urmator la fiecare aparitie a sa) se calculeaza matricea 
de contingenta. Pe baza acesteia se calculeaza o masura pentru cat de puternica
este alaturarea cuvantului respectiv cu fiecare dintre vecinii sai. In functie
de cea mai puternica alaturare se calculeaza costul arcului de la cuvant la
fiecare dintre vecinii sai.

Se citeste linie cu linie fisierul de intrare si se salveaza in vectori de
cuvinte cuvintele pentru care se doreste aflat costul, drumul minim si drumul de
lungime fixa si cost minim. Apoi se parseaza fisierul care contine textul cu
cuvintele car vor forma graful. Citesc linie cu linie fisierul si iterez peste
fiecare linie, salvand caracterele valide (adica litere, cifre si caracterul
apostrof) si transform majusculele in minuscule intr-un string. Daca am intalnit
un terminator de cuvant (spatiu, cratime, new line) atunci salvez cuvantul in
graf.

Dupa ce am citit toate cuvintele calculez costul de la fiecare nod la vecinul
sau in graf, apoi afisez costul dintre cuvintele din fisierul de intrare, drumul
de cost minim si drumul de lungime fixa si cost minim.

La incheierea programului eliberez memoria ocupata.


4. DETALII IMPLEMENTARE


	a) main.c

Aici sunt codata operatiile principale pentru functionarea programului: citirea
fisierului de input ce contine cuvintele pentru cerinte, parsarea textului si
construirea arborelui, rezolvarea cerintelor.

Functiile au fost concepute de asa natura incat sa aiba ca return code o
variabila a carei semnificatie este implementata in common.h prin intermediul 
enumerarii word _result. Verificarea codului returnat de o functie se face cu
macro-ul CHKRES care iese din program la intalnirea unui cod returnat de
o functie, cod diferit de WORD_SUCCESS. Codul returnat la terminarea neasteptata 
a programului este identic cu codul din enumerare, si, pe deasupra,
se incearca printarea mesajului corespunzator erorii, eroarea putand fi astfel 
interpretata de utilizator. In cazul functionarii anormale a programului, in
interiorul macro-ului, se incearca salvarea datelor in fisierul de iesire.


	b) common.h si common.c

Fisierul common.h contine constantele folosite de alte fisierele ale
programului, enumerarea word_result care contine return codes asteptate de la
functii. 

Tot aici este si functia de printare a mesajelor asociate enumerarii word_result 
si macro-urile folosite pentru debugging. DEBMSG tipareste un mesaj, precum si 
variabila ce contine mesajul, si DEBINFO tipareste expresia de evaluat precum si 
rezultatul evaluarii ei. Ambele tiparesc fisierul, linia si functia de unde au
fost apelate prin variabilele de compilator __FILE__, __LINE__ si __FUNCTION__. 
Macro-urile se activeaza explicit prin compilarea cu definirea constantei DEBUG 
(gcc -DDEBUG=1 <input> <output>).

Fisierul common.c contine implementarea functiei de tiparire a mesajului
corespunzator return code-urilor definite in enumerarea word_result.


	c) wgraph.h si wgraph.c

Aici este implementat graful. Graful de cuvinte este implementat cu o lista de
vecini. Deoarece el nu trebuie sa contina elemente duplicat, am implementat un
arbore binar de cautare care imi returneaza pozitia pe care trebuie inserat un
cuvant: indexul la care el se afla in graf sau o noua pozitie in graf dupa
ultimul element.

Graful este definit ca o structura ce contine un vector cu nodurile 
(vertex), precum si numarul total de cuvinte citite din fisier (nu neaparat 
distincte), numarul de cuvinte distincte (lungimea vectorului de noduri), 
memoria alocata pentru vectorul de cuvinte si indexul ultimului cuvant. Memorez
indexul ultimului cuvant pentru ca la matricea de contingenta, la calculul 
ponderii o12, el nu este urmat de niciun alt cuvant (fiind ultimul cuvant din 
text).

Nodurile sunt reprezentate ca o structura ce contine cuvantul respectiv, de cate
ori el apare in text si lista de vecini.

La adaugarea de noi elemente ma folosesc de arborele binar de cautare care imi
returneaza indexul pe care trebuie inserat noul cuvant.

La fel si pentru cautarea de cuvinte in graf ma folosesc de arborele binar. La
aflarea costului dintre doua cuvinte mai intai caut indexul sau din graf
folosind arborele, apoi parcurg lista de vecini si returnez costul. Daca unul
dintre cuvinte nu a fost gasit atunci returnez constanta INF (definita in
common.h) care reprezinta un cost infinit intre cuvinte.

Pentru calcularea drumului minim dintre doua cuvinte am folosit algoritmul de
Dijkstra cu o coada cu prioritati, implementata cu un heap de tip minim. La 
fiecare pas aleg nodul nevizitat cu drumul de cost minim, extragand minimul 
din coada cu prioritatii. Daca trecand prin noul nod de cost minim costul 
pana la alt nod se micsoreaza, salvez costul pana la nodul respectiv in 
vectorul de costuri si adaug nodul cu noul cost in coada cu prioritati. 
Procesul se repeta pana cand extrag din coada cu prioritati nodul care 
reprezinta finalul drumului.

Daca algoritmul se termina vizitand toate nodurile cu vecini, dar nu am trecut 
prin nodul cautat inseamna ca nodul este izolat.

Bonusul a fost implementat folosind metoda backtracking si implementarea este
descrisa in sectiunea BONUS.


	d) tree.h si tree.c

Aici este implementat arborele binar de cautare ce retine cuvintele si indexul
lor in graful de cuvinte. Cuvintele sunt comparate lexicografic la parcurgerea
arborelui.


	e) list.h si list.c

Aici este implementata lista cu vecini ca o lista simplu inlantuita. Vecinii
sunt reprezentati printr-o structura ce contine numarul lor de aparitii, indexul
lor in graf si costul drumului de la nodul din graf pana la vecinul respectiv.

Retin indexul lor in graf (spre deosebire de cuvantul propriu zis) pentru o
parcurgere mai rapida a drumurilor (si pentru o economie de memorie).

	
	f) pqueue.h si pqueue.c

In pqueue.h este declarata structura pqueue, implementata ca un obiect ce
contine pointeri catre functiile care realizeaza inserarea de noi noduri din
graf, de extragere a minimului, de verificare daca lista este goala, de
actualizare a costului unui nod (util la calculul drumului de cost minim cu
algoritmul lui Dijkstra) si de printare a listei, precum si functiile 
constructor si destructor (facand analogie cu un obiect) pentru variabila de 
tip pqueue.

Intern, coada cu prioritati este implementata cu un heap ce contine un vector 
in care se retine indexul in graf si costul pana la nodul respectiv. Pentru o
cautare rapida a unui cuvant in heap (necesara in cazul actualizarii costurilor
la algoritmul lui Dijkstra) am folosit un vector suplimentar in care indexul 
reprezinta nodul din graf, iar valoarea indexul in heap.

In pqueue.c sunt implementate functiile necesare structurii pqueue, precum si
alte functii auxiliare. Toate functiile sunt declarate static pentru a nu putea
fi accesate de alte fisiere decat prin intermediul structurii pqueue.c.

Inserarea se face cu urmatorul algoritm: elementul este copiat pe ultima 
pozitie din heap, apoi este filtrat in sus pentru a se respecta conditiile de 
heap, adica elementul de pe pozitia k este mai mic decat fiul stang (de pe 
pozitia 2k+1) si cel drept (de pe pozitia 2k+2), unde k este indicele 
elementului in vector (filtrare realizata de functia sift_up).

Extragerea minimului se realizeaza prin stergerea elementului de pe pozitia 0
prin inlocuirea acestuia cu elementul de pe ultima pozitie. Se decrementeaza
lungimea heap-ului, si apoi elementul de pe pozitia 0 (fost ultim) se filtreaza
in jos pana la respectarea conditiilor de heap (filtrare ealizata de sift_down).


5. BONUS

Pentru rezolvarea bonusului am folosit metoda backtracking. Mai intai inversez
graful si astfel cuvant cu care trebuie sa se sfarseasca calea devine primul.
Apoi pornind de la acesta vizitez toti vecinii lui, toti vecinii vecinilor si
tot asa pana cand am vizitat numarul cerut de vecini. Daca am un cost mai mic
decat costul minim atunci suprascriu drumurile precedente cu noul drum. Daca am
un cost egal (in limitele preciziei definite in common.h prin constanta
PRECISION) atunci adaug noul drum vectorului care retine drumurile cu acelasi
cost minim.

#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include <cassert>
#include "gsl/span"
#include <memory>
#include <functional>
#include <limits>
#include <algorithm>
using gsl::span;
using namespace std;
struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class ListeFilms {
public:
	ListeFilms() = default;
	ListeFilms(const std::string& nomFichier);
	ListeFilms(const ListeFilms& l) { assert(l.elements == nullptr); } // Pas demandé dans l'énoncé, mais on veut s'assurer qu'on ne fait jamais de copie de liste, car la copie par défaut ne fait pas ce qu'on veut.  Donc on ne permet pas de copier une liste non vide (la copie de liste vide est utilisée dans la création d'un acteur).
	~ListeFilms();
	void ajouterFilm(Film* film);
	void enleverFilm(const Film* film);
	shared_ptr<Acteur> trouverActeur(const std::string& nomActeur) const;
	span<Film*> enSpan() const;
	int size() const { return nElements; };
	Film* operator[](int index) { return elements[index]; };
	Film* trouverFilm(const function<bool(Film)>& critere);

private:
	void changeDimension(int nouvelleCapacite);

	int capacite = 0, nElements = 0;
	Film** elements = nullptr; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
	bool possedeLesFilms_ = false; // Les films seront détruits avec la liste si elle les possède.
};

template <typename T> class Liste
{
public:
	Liste() {};
	Liste(int capacite, int nElements) : capacite_(capacite), nElements_(nElements)
	{
		elements_ = make_unique<shared_ptr<T>[]>(capacite);
	};
	Liste<T>(Liste<T> const& ancListe)
	{
		//assert(ancListe.elements_ == nullptr);
		capacite_ = ancListe.capacite_;
		nElements_ = ancListe.nElements_;
		elements_ = make_unique<shared_ptr<T>[]>(capacite_);
		for (int i = 0; i < ancListe.nElements_; i++)
		{
			elements_[i] = ancListe.elements_[i];
		}
	};
	Liste<T>& operator=(Liste<T>&& ancListe)
	{
		capacite_ = ancListe.capacite_;
		nElements_ = ancListe.nElements_;
		elements_ = make_unique<shared_ptr<T>[]>(capacite_);
		for (int i = 0; i < ancListe.nElements_; i++)
		{
			elements_[i] = ancListe.elements_[i];
		}
		return *this;
	}
	int lireCapacite()
	{
		return capacite_;
	}
	int lireNElements()
	{
		return nElements_;
	}
	void changeDimension(int nouvelleCapacite)
	{
		auto nouvelleListe = make_unique<shared_ptr<T>[]>(nouvelleCapacite);

		if (elements_ != nullptr)
		{  // Noter que ce test n'est pas nécessaire puique nElements sera zéro si elements est nul, donc la boucle ne tentera pas de faire de copie, et on a le droit de faire delete sur un pointeur nul (ça ne fait rien).
			nElements_ = min(nouvelleCapacite, nElements_);
			for (int i = 0; i < nElements_; i++)
			{
				nouvelleListe[i] = elements_[i];
			}
		}
		elements_ = move(nouvelleListe);
		capacite_ = nouvelleCapacite;
	}

	void ajouterElement(const shared_ptr<T>& nouveau)
	{
		if (nElements_ == capacite_)
		{
			changeDimension(max(1, capacite_ * 2));
		}
		elements_[nElements_++] = nouveau;
	}
	shared_ptr<T>* lireElements()
	{
		return elements_.get();
	};
	span<shared_ptr<T>> enSpan() const
	{
		return span(elements_.get(), nElements_);
	}
	shared_ptr<T>& operator[](int index) const
	{
		return elements_[index];
	}

private:
	int capacite_ = 0;
	int nElements_ = 0;
	unique_ptr<shared_ptr<T>[]> elements_ = make_unique<shared_ptr<T>[]>(capacite_);
};
using ListeActeurs = Liste<Acteur>;

class Item
{
private:
	std::string titre_ = "";
	int annee_ = 0;
public:
	Item() {};
	Item(std::string titre, int annee):titre_(titre), annee_(annee) {};
	std::string lireTitre()
	{
		return titre_;
	};
	int lireAnnee()
	{
		return annee_;
	};
	void modifierTitre(std::string nouveau)
	{
		titre_ = nouveau;
	};
	void modifierAnnee(int nouvelle)
	{
		annee_ = nouvelle;
	}
};

class Livre : public Item
{
private:
	std::string auteur_ = "";
	int ventes_ = 0;
	int pages_ = 0;
public:
	Livre():Item() {};
	Livre(std::string titre, int annee, std::string auteur, int ventes, int pages):
		Item(titre,annee), 
		auteur_(auteur), 
		ventes_(ventes), 
		pages_(pages) 
	{};
	std::string lireAuteur()
	{
		return auteur_;
	};
	int lireVentes()
	{
		return ventes_;
	};
	int lirePages()
	{
		return pages_;
	};
	void modifierAuteur(std::string nouveau)
	{
		auteur_ = nouveau;
	};
	void modifierVentes(int nouvelle)
	{
		ventes_ = nouvelle;
	}
	void modifierPages(int nouvelle)
	{
		pages_ = nouvelle;
	}
};
class Film : public Item
{
private:
	std::string realisateur_ = "";
	int recette_ = 0;
	ListeActeurs acteurs_ = ListeActeurs();
public:
	Film() :Item() {};
	Film(std::string titre, int annee, std::string realisateur, int recette, ListeActeurs acteurs) :
		Item(titre, annee),
		realisateur_(realisateur),
		recette_(recette),
		acteurs_(acteurs)
	{};
	Film(const Film& autre)
	{
		realisateur_ = autre.realisateur_;
		recette_ = autre.recette_;
		acteurs_ = ListeActeurs(autre.acteurs_);
	}
	Film& operator=(const Film&& autre)
	{
		if (this != &autre)
		{
			realisateur_ = autre.realisateur_;
			recette_ = autre.recette_;
			acteurs_ = ListeActeurs(autre.acteurs_);
		}
		return *this;
	}
	std::string lireRealisateur() 
	{
		return realisateur_;
	};
	int lireRecette() const
	{
		return recette_;
	};
	ListeActeurs lireActeurs() 
	{
		return acteurs_;
	};
	void modifierRealisateur(std::string nouveau) 
	{
		realisateur_ = nouveau;
	};
	void modifierRecette(int nouvelle) 
	{
		recette_ = nouvelle;
	}
	void modifierActeurs(ListeActeurs nouvelle) 
	{
		acteurs_ = ListeActeurs(nouvelle);
	}
};

//struct Film
//{
//	std::string titre = "", realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
//	int anneeSortie = 0, recette = 0; // Année de sortie et recette globale du film en millions de dollars
//	ListeActeurs acteurs;
//	Film() = default;
//	Film(const Film& autre)
//	{
//		titre = autre.titre;
//		realisateur = autre.realisateur;
//		anneeSortie = autre.anneeSortie;
//		recette = autre.recette;
//		acteurs = ListeActeurs(autre.acteurs);
//	}
//	Film& operator=(const Film&& autre)
//	{
//		if (this != &autre)
//		{
//			titre = autre.titre;
//			realisateur = autre.realisateur;
//			anneeSortie = autre.anneeSortie;
//			recette = autre.recette;
//			acteurs = ListeActeurs(autre.acteurs);
//		}
//		return *this;
//	}
//};

struct Acteur
{
	std::string nom = ""; int anneeNaissance = 0; char sexe='x';
	//ListeFilms joueDans;
};
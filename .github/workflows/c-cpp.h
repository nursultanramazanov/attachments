name: C/C++ CI

on: cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css ../css/bootstrap.min.css;
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js ../js/bootstrap.min.js;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.eot ../fonts/glyphicons-halflings-regular.eot;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.svg ../fonts/glyphicons-halflings-regular.svg;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.ttf ../fonts/glyphicons-halflings-regular.ttf;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff ../fonts/glyphicons-halflings-regular.woff;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff2 ../fonts/glyphicons-halflings-regular.woff2;

cp vendor/select2/select2/select2.js ../js/select2.js;
cp vendor/select2/select2/select2_locale_ru.js ../js/select2_locale_ru.js;
cp vendor/select2/select2/select2.css ../css/select2.css;
cp vendor/select2/select2/select2.png ../css/select2.png;
cp vendor/select2/select2/select2-spinner.gif ../css/select2-spinner.gif;

cp vendor/silverfire/select2-bootstrap3-css/select2-bootstrap.min.css ../css/select2-bootstrap.min.css;
  push: """
Reuires a webbrowser and the helium module
The hashlib module isn't needed basically as I had to find an easy way to get different a password for each account
BUT, kartable.fr accepts pretty much every fuckin' email, name, password and birthdate without complaining
so you could create an account with a non-existing email

Most of the features are unstable and I'll certainly add a log file to handle errors
"""
import helium
import hashlib
import time
from selenium.webdriver.support.ui import Select

class main:
    def __init__(self, classe, firstname, lastname, email, password, birthdate=None, ecole="victor hugo", url="https://www.kartable.fr/inscription"):
        # birthdate as follows : DD/MM/YYYY
        self.url = url
        self.classe = classe
        self.firstname = firstname
        self.lastname = lastname
        self.email = email
        self.password = password
        self.ecole = ecole
        self.birthdate = birthdate.split("/") if len(str(birthdate).split("/")) == 3 else None
        self.driver = helium.start_chrome(self.url)

    def register(self, sponso=False):
        if sponso == False:
            try:
                helium.click("élève")
                helium.click("élève")
            except LookupError:
                print("élève failed")
        helium.click(self.classe)
        helium.click("s'inscrire avec un e-mail")
        helium.write(self.firstname, into="prénom")
        helium.write(self.lastname, into="nom")
        helium.write(self.email, into="adresse e-mail")
        helium.write(self.password, into="mot de passe")
        # Setting custom birthdate
        self.set_birthdate()
        helium.click("terminer")
        # time.sleep(5)
        try:
            helium.click("plus tard")
        except LookupError:
            pass
        time.sleep(5)

    def log_out(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.get("https://www.kartable.fr/compte")
        helium.click("Se déconnecter")
        helium.click("SE DÉCONNECTER")

    def log_in(self):
        self.driver.get("https://www.kartable.fr/connexion")
        helium.write(self.email, into="adresse e-mail")
        helium.write(self.password, into="mot de passe")
        helium.click("se connecter")

    def join_school(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.get(
            "https://www.kartable.fr/classe?navigationOrigin=ruby-quest")
        helium.write(self.ecole, into="Rechercher un établissement...")
        select = Select(self.driver.find_element_by_tag_name('select'))
        select.select_by_value('1')  # select the first
        helium.click("rejoindre")
        self.driver.find_element_by_class_name("icon-close").click()

    def delete_school(self):
        return "Not implemented yet"

    def change_email(self, new_email):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-adresse-email
        for i in self.driver.find_element_by_tag_name("input"):
            helium.write(new_email, into=i)
        helium.click("mettre à jour")

    def change_password(self, new_password):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-mot-de-passe
        helium.write(self.password, into="Mot de passe actuel")
        helium.write(new_password, into="Nouveau mot de passe")
        helium.write(new_password, into="Ressaisir nouveau mot de passe")
        helium.click("mettre à jour")

    def is_locked(self, ressource):
        return "Not implemented yet"
        # self.driver.find_element_by_class_name("document__state")#if child has class icon-document-locked :True

    @property
    def current_email(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-adresse-email
        return self.driver.find_element_by_tag_name("strong").text

    def delete_account(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles
        helium.click("SUPPRIMER LE COMPTE")
        helium.click("SUPPRIMER DÉFINITIVEMENT")

    def levels_alternative(self):
        return "Not implemented yet"
        # requires status: not signed in
        # requires url: https://www.kartable.fr/
        tmp = self.driver.find_element_by_class_name("et_pb_text_inner")
        for i in tmp:
            if "Classes" == tmp.find_element_by_class_name("links-list__title"):
                # tmp.find_element_by_tag_name("a").href +.text
                pass

    @property
    def account_info(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles
        firstname = self.driver.find_element_by_xpath(
            "//input[@name='firstName']")
        lastname = self.driver.find_element_by_xpath(
            "//input[@name='lastName']")
        birthdate = ""
        input__group = self.driver.find_element_by_class_name(
            "input__group--date")  # includes a bunch of select tags
        select = input__group.find_elements_by_tag_name('select')
        for i in select:
            birthdate += Select(i).first_selected_option.text+"/"
        tmp = ""
        for i in birthdate[:-1].split("/"):
            tmp += "0"+i+"/" if len(i) == 1 else i + "/"
        birthdate = tmp[:-1]
        return {"firstname": firstname, "lastname": lastname, "birthdate": birthdate}

    def set_birthdate(self):
        #!!!!!NOT TESTED YET!!!!!!
        if self.birthdate != None:
            day_to_be_set = self.birthdate[0]
            month_to_be_set = self.birthdate[1]
            year_to_be_set = self.birthdate[2]
            try:
                input__group = self.driver.find_element_by_class_name(
                    "input__group--date")  # includes a bunch of select tags
                select = input__group.find_elements_by_tag_name('select')
                for item in select:
                    if item.get_attribute("name") == "birthDateDay":
                        day = Select(item)
                        day.select_by_value(day_to_be_set)
                    elif item.get_attribute("name") == "birthDateMonth":
                        month = Select(item)
                        month.select_by_value(month_to_be_set)
                    elif item.get_attribute("name") == "birthDateYear":
                        year = Select(item)
                        year.select_by_value(year_to_be_set)
            except Exception as e:
                print("Failed to set birthdate", e)

    def subject_categories(self):
        return "Not implemented yet"
        #mm = [i for i in self.driver.find_elements_by_class_name("category__link")]

    def delete_premium_ads(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.execute_script(
            'try{document.getElementsByTagName("premium-link")[0].remove();}catch(error){console.error(error);}')
        self.driver.execute_script(
            'try{document.getElementsByClassName("cross-selling-ad")[0].parentElement.remove();}catch(error){console.error(error);}')

    @property
    def has_school(self):
        return "Not implemented yet"

    @property
    def subjects(self, classe):
        #!!!!!NOT TESTED YET!!!!!!
        self.unavailable = []
        self.courses_info = []
        # let it redirect to homepage to avoid spelling mistakes (instead of get("https://www.kartable.fr/cinquieme") for eg)
        self.driver.get("https://www.kartable.fr/")
        try:
            self.courses_info = [{i.text: i.get_attribute("href")} for i in self.driver.find_elements_by_class_name(
                "course__link")]  # href is None if not available
        except Exception as e:
            print("Failed to get course titles", e)
            self.courses_info = False

        try:
            for i in self.driver.find_elements_by_class_name("course--coming-soon"):
                self.unavailable.append(
                    i.find_element_by_class_name("course__link").text)
        except Exception as e:
            print("Failed to get name of future available subjects")
            self.unavailable = False
        return self.courses_info+self.unavailable

    @property
    def levels(self):
        return "Not implemented yet"
        """https://www.kartable.fr/{classe}"""
        helium.click(self.current_level)
        m = list(set(
            [i.text for i in self.driver.find_elements_by_class_name("school-year__level")]))
        self.driver.find_element_by_class_name("icon-close").click()

    @property
    def current_level(self):
        try:
            level = self.driver.find_element_by_class_name(
                "school-year__level").text
        except Exception as e:
            print("Failed to get the level", e)
            level = False
        finally:
            return level

    @property
    def sponso_url(self):
        self.driver.get(
            "https://www.kartable.fr/compte/parrainage?questIdentifier=sponsorship_fifth")
        time.sleep(5)
        self.driver.execute_script(
            'try{document.getElementsByTagName("input")[0].removeAttribute("readonly");}catch(error){console.error(error);}')
        self._sponso_url = self.driver.execute_script(
            'return document.getElementsByTagName("input")[0].value;')
        if not self._sponso_url.startswith("https://"):
            return False
        return self._sponso_url

    @property
    def ruby_amount(self):
        """Works for https://www.kartable.fr/rubis, https://www.kartable.fr/cinquieme, https://www.kartable.fr/ (because redirects to .../{classe}"""
        try:
            rubies = self.driver.find_element_by_class_name(
                "ruby-counter__total").text
        except Exception as e:
            print("Failed to get the amount of rubies", e)
            rubies = False
        finally:
            return rubies


"""
Usage:
    instance = main("6e","philotea","rotrions","manlymprinc1241478451ent@gmail.com","xfcgvhibu1457hoomskpjoihugyf")
    time.sleep(5)
    instance.register()
    ...
    url = instance.sponso_url
    instance.log_out()

"""
    branches: [ "main" ]
  pull_request: 
#include "MoveCommand.h"

static const std::string _name("move");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e4\n"
            "Description: Moves the selected piece to the specified square if legal.";

MoveCommand::MoveCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result MoveCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.move({row, column});
    return {error, false};
}
    branches: [ "main" ]

jobs: #pragma once

#include "../Command.h"

// Moves the selected piece to a new square.
// Params:
//      1. string of format {char}{int} representing a
//          field of the chessboard.
class MoveCommand : public Command
{
public:
    MoveCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
  build: 
#include "QuitCommand.h"

static const std::string _name = "quit";
static const std::string _description =
    "Arguments: [None]\n"
    "Description: Quits the game.";

QuitCommand::QuitCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result QuitCommand::apply(const std::vector<std::string> &params)
{
    bool error = !params.empty();
    bool quit = true;
    return {error, quit};
}

    runs-on: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};

    steps: 
#include "SelectCommand.h"

static const std::string _name("select");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e2\n"
            "Description: Selects the specified square.";

SelectCommand::SelectCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result SelectCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.select({row, column});
    return {error, false};
}
    - uses: #pragma once

#include "../Command.h"

// Selects a square.
// Params:
//     1. row
//     2. column
class SelectCommand : public Command
{
public:
    SelectCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
    - name: 
#include "Command.h"

Command::Command(Chess &chess, std::string name, std::string description)
    : chess(chess), name(name), description(description) {}

const std::string &Command::get_name() const
{
    return this->name;
}

const std::string &Command::get_description() const
{
    return this->description;
}

Chess &Command::get_chess()
{
    return this->chess;
}
      run: ./#pragma once

#include <string>
#include <vector>

#include "Result.h"

#include "../model/Chess.h"

class Command
{
public:
    Command(Chess &chess, std::string name, std::string description);
    virtual ~Command() = default;

    const std::string &get_name() const;
    const std::string &get_description() const;

    virtual Result apply(const std::vector<std::string> &params) = 0;

protected:
    Chess &get_chess();

private:
    Chess &chess;
    std::string name;
    std::string description;
};
    - name: #pragma once

struct Result
{
    bool error;
    bool quit;
};
      run: import sys

print("")
print("..............CONSIGNES.................")
print("  Ce script va calculer votre moyenne pour vous!")
print("Il faudra lui spécifier le nom de la matiere et le nombre de notes que vous avez eu pour cette matière")
print("Ne taper que des nombres")
print("..........................................")
print("")
print("")
print("Voici le choix de la matière :")
print("{1} Maths")
print("{2} Français")
print("{3} Anglais")
print("{4} Espanol")
print("{5} Sport")
print("{6} Technologie")
print("{7} Physique")
print("{8} SVT")
print("{9} Histoire-Géo")
print("{10} Religion")
try:
        choice = int(input("Quel matière choisissez-vous?"))
except ValueError:
        print("Vous n'avez pas taper un nombre!")
        print("Au revoir...")
        input("Taper la touche Entrée pour quitter")
        sys.exit(1)
else:
        if choice == 1:
                choice = "Maths"
        elif choice == 2:
                choice = "Français"
        elif choice == 3:
                choice = "Anglais"
        elif choice == 4:
                choice = "Espagnol"
        elif choice == 5:
                choice = "Sport"
        elif choice == 6:
                choice = "Technologie"
        elif choice == 7:
                choice = "Physique"
        elif choice == 8:
                choice = "SVT"
        elif choice == 9:
                choice = "Histoire-Géo"
        elif choice == 10:
                choice = "Religion"
        else:
                print("Ceci ne correspond pas aux matières proposées!")
                print("Au revoir")
                input("Taper la touche Entrée pour quitter")
                sys.exit(1)
        print("Vous avez choisi de calculer votre moyenne pour:", choice)
        print("")
        print("..........................................")
        print("")
        try:
                nbnot = int(input("Combien avez vous eu de notes ?"))
        except ValueError:
                print("Vous n'avez pas taper un nombre!")
                print("Au revoir...")
                input("Taper la touche Entrée pour quitter")
                sys.exit(1)
        else:
                if nbnot == 1 or nbnot >= 50:
                        print("Ce nombre de note est incorrect!")
                        input("Appuyer sur la touche Entrée pour quitter")
                        sys.exit(1)
                note = []
                coef = []
                print("Appuyer sur la touche Entrée après chaque notes")
                for i in range(nbnot):
                        try:
                                i =  float(input("Taper une note: "))
                                p = float(input("Quel est son coefficient?"))
                        except ValueError:
                                print("Vous n'avez pas taper un nombre!")
                                print("Au revoir...")
                                input("Taper la touche Entrée pour quitter")
                                sys.exit(1)
                        else:
                                coef.append(p)
                                i = i * p
                                note.append(i)
                add = sum(note)#sum() additionne les valeurs de note entre elles
                ad = sum(coef)# addtions des coefficients
                calc = add/ad
# Pour calculer une moyenne:
# additionner les notes
# additionner les coefficients
# diviser le premier résultat avec le deuxième
                if round(calc) == calc:
                        calc = int(calc)
                else:
                        calc = float(calc)
                print("")
                print("[..........  ...........................]")
                print("Votre moyenne en", choice,"est de", float(round(calc, 2)))
                print("[.......................................]")
                print("")
                print("")
                input("Appuyer sur la touche Entrée pour quitter")
    - name: import java.util.Scanner;

public class matutil {
        public static void main(String[] args) {
                Scanner reader = new Scanner(System.in);
                System.out.println("Taper un nombre: ");
                double nb = reader.nextDouble();
            System.out.println("................................................................");
                for (int i=1; i <= nb+1; i++) {
                if (nb/i == Math.round(nb/i)){
                        System.out.println(nb + " est divisible par " + i);
                    System.out.println(nb + " divise par " + i + " vaut " + nb/i);
                System.out.println(" ");
                } else {
                    continue; }
        }
            for (int x = 1; x <= 28; x++) {
                System.out.println(x + " fois " + nb + "= " + x*nb);
    }
    System.out.println(" ");
    System.out.println("Carre: " + nb*nb);
    System.out.println(" ");
    if (Math.sqrt(nb) == Math.round(Math.sqrt(nb))){
        System.out.println("La racine carree de " + nb + " tombe juste");
        System.out.println(" ");
    } else{
        System.out.println("La racine carree de "  + nb + " ne tombe pas juste");
            System.out.println("Racine carree: " + Math.sqrt(nb));
        System.out.println(" ");
    }
    for (int v=1; v <= 11; v++) {
        System.out.println("Exposant " + v +": " + Math.pow(nb, v));
        }
        }
}
      run: import sys
from math import sqrt


def main():
    for i in range(1, nb+1):
        if nb/i == round(nb/i):
            print(nb, "est divisible par", i,)
            print(nb, "divisé par", i, "vaut", nb/i)
        else:
            pass
    for x in range(1, 21):
        print(x, "×", nb, "=", x*nb)
    print("Carré:", nb*nb)
    if sqrt(nb) == round(sqrt(nb)):
        print("La racine carrée de", nb, "tombe juste")
    else:
        print("La racine carrée de" , nb, "ne tombe pas juste")
    print("Racine carrée:", round(sqrt(nb)))
    for v in range(1,11):
        print("Exposant",v,":", nb ** v)
        v += 1

try:
    nb = float(input("Taper un nombre =>"))
    print("......................................")
except ValueError:
    print("Vous n'avez pas tapé un nombre")
    sys.exit()
else:
    if nb == int(nb):
        nb = int(nb)
        main()
    else:
        nb = float(nb)
        main()
    - name: import sys


nbpremiers = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 43, 59, 61, 67, 71]
print("PGCD: Plus Grand Diviseur Commun")

def Euclide(a, b):
        while a%b != 0:
                a, b = b, a%b
                maxi = max(a, b)
                mini = min(a, b)
                reste = maxi%mini
                div = int(maxi/mini)
                print("{0} = {1}*{2} + {3}".format(maxi, mini, div, reste))
                maxi = mini
                mini = reste
        Back()

def Back():
        print("................................................................................")
        print()
        print("1- Revenir au menu principal")
        print("2- Quitter")
        print()
        try:
                re = int(input("Taper le nombre de votre choix => "))
        except ValueError:
                print("Vous n'avez pas taper un nombre valable")
                sys.exit()
        if re == 1:
                Menu()
        elif re == 2:
                print("Au revoir.")
                sys.exit()
        else:
                print("Ce nombre ne correspond pas aux choix disponibles")
                sys.exit()

def PGCD(A, B):
        if A <= 0 or B <= 0:
                print("Les nombres ne peuvent pas être égaux ou inférieur à 0")
                sys.exit()
        elif A >= 100000000 or B >= 100000000:
                print("Les nombres ne peuvent pas être au-dessus de 1000")
                sys.exit()
        divA = []
        divB = []
        divcom = []
        for x in range(1, A+1):
                calc = int(A)/x
                if calc == int(round(calc)):
                        if x in divA:
                                pass
                        else:
                                divA.append(int(x))
                else:
                        pass
        for i in range(1,B+1):
                ca = int(B)/i
                if ca == int(round(ca)):
                        if i in divB:
                                pass
                        else:
                                divB.append(i)
                else:
                        pass
        for nb in range(max(A, B)+1):
                if nb in divA and nb in divB:
                        if nb in divcom:
                                pass
                        else:
                                divcom.append(nb)
                else:
                        pass
        print("................................................................................")
        print("Liste des diviseurs de", A)
        print(divA)
        print("")
        print("Liste des diviseurs de", B)
        print(divB)
        print("")
        print("Liste des diviseurs communs")
        print(divcom)
        longueur = max(divcom)
        print("D'où le plus grand diviseur commun de {0} et de {1} est:".format(A, B), longueur)
        Back()

def View(a, b):
        if a != int(a) and b != int(b):
                print("Tape des entiers!")
                sys.exit()
        if a > b:
                grand = a
                g = b
        elif b > a:
                grand = b
                g = a
        print("PGCD({0}, {1})".format(a, b))
        while True:
                petit = max(a, b) - min(a, b)
                print("= PGCD({0}, {1})".format(min(a, b), petit))
                ok = min(a, b)
                encor = max(ok, petit) - min(petit, ok)
                print("= PGCD({0}, {1})".format(petit, encor))
                a = petit
                b = encor
                if petit == encor:
                        break
                if max(a, b) <= 0 or min(a, b) <= 0 or petit <= 0:
                        break
        Back()

def Menu():
        print()
        print()
        print("......................................MENU......................................")
        print()
        print("1- Methode d'Euclide")
        print("2- Methode par soustractions successives")
        print("3- Methode détaillée")
        print()
        try:
                choix = int(input("Taper le nombre correspondant à votre choix => "))
        except ValueError:
                print("Vous n'avez pas taper un nombre valable")
                sys.exit()
        print("..................................................................................")
        try:
                if choix == 1:
                        a = int(input("Taper A: "))
                        b = int(input("Taper B: "))
                        Euclide(a, b)
                elif choix == 2:
                        a = int(input("Taper A: "))
                        b = int(input("Taper B: "))
                        View(a, b)
                elif choix == 3:
                        A = int(input("Taper A: "))
                        B = int(input("Taper B: "))
                        PGCD(A, B)
                else:
                        print("Ce nombre ne correspond pas aux choix disponibles")
        except ValueError:
                print("Vous n'avez pas taper un nombre valable")
                sys.exit()
Menu()
      run: u = {} # {0:1, 1:2, 2:5, 3:26,...}


def recurrence_sequence(first_n: int, first_u_n: int, n_seeked: int):
    """
    Parameters: first_n corresponds to the value of the initial term of the sequence whose image is known by Un.
                first_u_n is the image of this initial term by Un
                n_seeked is the value n searched for
    Example: U0 = 1
              0 would then be the value of the "first_n" parameter, and
              1 the value of "first_u_n".
    """
    for n in range(first_n, n_seeked+1):
        u[n] = u[n-1]*2 - 3 if n-1 in u.keys() else first_u_n
        yield f"U{n} = {u[n]}"


for i in recurrence_sequence(0, 1, int(input("n value seeked: "))):
    print(i)

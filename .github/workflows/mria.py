name: Rust

on: @startuml

bootstrap: Receiving all the records\n from the core node
local_replay: Replaying transactions\n that have been buffered locally\n during bootstrap
normal: Remote transactions are applied\ndirectly to the replica

[*] --> bootstrap
bootstrap --> local_replay : received bootstrap_complete
local_replay --> normal : reached the end of the local rlog

@enduml
  push: @startuml
scale 2000 width

participant "RLOG server" as server #ffc
participant "RLOG agent" as agent #ffc
participant "RLOG bootstrapper" as boot_serv #ffc

participant "RLOG replica" as repl #ccf
participant "RLOG replica\nimporter worker" as repl_imp #ccf
participant "bootstrap client" as boot_client #ccf

activate server
activate repl
activate repl_imp

group Agent initialization
== Probe the connection to minimize risk of double-subscription and negotiate protocol version ==
  repl -> server : probe
  repl <- server : {true, 0}
  note over repl: The protocol version\nshould be the same,\nabort connection if different

== Initiate connection ==
  repl -> server : {connect, LocalCheckpointTS}
  note over server : Spawn a new agent process\nunder the shard supervisor
  server -\\ agent : spawn(now() - SafeInterval)

  activate agent

  repl <- server : {need_bootstrap, AgentPID}
end

== Bootstrapping ==

group Bootstraper initialization
  hnote over repl : bootstrap

  repl -\\ boot_client : spawn()
  activate boot_client

  boot_client -> server : {bootstrap, self()}
  server -\\ boot_serv : spawn(RemotePid)
  activate boot_serv

  boot_serv -> boot_serv : mnesia:dirty_all_keys\nfor each table in shard

  server -> boot_client : {ok, Pid}
end

group Bootstrap
  note over boot_serv : Iterate through the\ncached keys
  loop
    boot_serv -> boot_client : {batch, [{Tab, Record}]}
    boot_client -> boot_client : import batch to the\ntable replica
    boot_serv <- boot_client : ok
  end

  note over agent : At the same time...

  loop
    server -> agent : {trans, SeqNo, TLOG}
    agent -> repl : '#entry{}'
    repl -> repl : cache batch to the local rlog
  end

  boot_serv -> boot_client : bootstrap_complete
  deactivate boot_serv
  boot_client -> repl : bootstrap_complete
  deactivate boot_client
end

group local_replay
  hnote over repl : local_replay

  note over repl : Iterate through the\ncached transactions

  loop
    server -> agent : {trans, SeqNo, TLOG}
    agent -> repl : '#entry{}'
    repl -> repl : cache batch in the local rlog

    repl -> repl : Import ops from the local rlog\nto the local replica
  end

  note over repl : Reached the end of\nthe local rlog
end


== Normal operation ==

hnote over repl : normal

loop
  server -> agent : {trans, SeqNo, TLOG}
  agent -> repl : '#entry{SeqNo}'
  server -> agent : {trans, SeqNo + 1, TLOG}
  agent -> repl : '#entry{SeqNo + 1}'
  server -> agent : {trans, SeqNo + 2, TLOG}
  agent -> repl : '#entry{SeqNo + 2}'
  repl -> repl_imp : {import_batch, [Entries 1 2 3]}
  repl_imp -> repl_imp : Import batch to the\nlocal replica
  repl <- repl_imp : ok
end

@enduml
    branches: [ "main" ]
  pull_request: -type(cluster() :: atom()).

-type(member_status() :: joining | up | healing | leaving | down).

-type(member_address() :: {inet:ip_address(), inet:port_number()}).

-record(member, {
          node   :: node(),
          addr   :: undefined | member_address(),
          guid   :: undefined | mria_guid:guid(),
          hash   :: undefined | pos_integer(),
          status :: member_status(),
          mnesia :: undefined | running | stopped | false,
          ltime  :: undefined | erlang:timestamp(),
          role   :: mria_rlog:role()
         }).

-type(member() :: #member{}).

-define(JOIN_LOCK_ID, {mria_sync_join, node()}).
    branches: [ "main" ]

env: #!/bin/bash
set -euxo pipefail

echo "Building "${tag}""

[ -d "${tmp_dir}" ] ||
    git clone "${root_dir}" "${tmp_dir}"

cd "${tmp_dir}"
git checkout "${tag}"

# Avoid rebuilding rocksdb:
mkdir -p "_checkouts"
pushd "_checkouts"
[ -L rocksdb ] ||
    ln -s "${root_dir}/_build/default/lib/rocksdb" rocksdb
popd

[ ! -z $(find . -name mria.app) ] ||
    rebar3 as test compile
  CARGO_TERM_COLOR: import sys

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
jobs: import java.util.Scanner;

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
  build: import sys
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

    runs-on: import sys


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

    steps: u = {} # {0:1, 1:2, 2:5, 3:26,...}


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
    - uses: actions/checkout@v4
    - name: Build
      run: cargo build --verbose
    - name: Run tests
      run: cargo test --verbose

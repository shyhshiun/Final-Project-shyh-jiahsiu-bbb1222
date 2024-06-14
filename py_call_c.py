import os
import ctypes
from ctypes import Structure, c_size_t, c_char_p, c_int, POINTER, byref



class Choice(Structure):
    _fields_ = [("desc", c_char_p),
                ("outcome", c_char_p)]


class Option(Structure):
    _fields_ = [("text", c_char_p),
                ("next", c_char_p)]


class Act(Structure):
    _fields_ = [("type", c_char_p),
                ("name", c_char_p),
                ("scene", c_char_p),
                ("desc", c_char_p),
                ("choices", POINTER(Choice)),
                ("n_choice", c_size_t),
                ("text", c_char_p),
                ("options", POINTER(Option)),
                ("n_option", c_size_t),
                ("character", c_char_p),
                ("dialogue", c_char_p)]


# set the type of parameter and return value
lib = ctypes.CDLL(f'{os.getcwd()}/libmyc.so')

lib.read_toml.argtypes = [POINTER(POINTER(Act)), POINTER(c_int), c_char_p]
lib.read_toml.restype = c_int

lib.print_Act2.argtypes = [POINTER(Act)]
lib.print_Act2.restype = None

lib.free_Acts.argtypes = [POINTER(Act), c_int]
lib.free_Acts.restype = None



def main():
    path = b'sample_our.toml'
    acts = POINTER(Act)()
    n_act = c_int()

    lib.read_toml(byref(acts), byref(n_act), path)
    
    # print
    print('n act: ', n_act.value)
    for i in range(n_act.value):
        act = acts[i]
        print(f"段落 {i + 1}:")
        print(f"  type: {act.type.decode('utf-8') if act.type else None}")
        print(f"  name: {act.name.decode('utf-8') if act.name else None}")
        print(f"  scene: {act.scene.decode('utf-8') if act.scene else None}")
        print(f"  desc: {act.desc.decode('utf-8') if act.desc else None}")
        print(f"  text: {act.text.decode('utf-8') if act.text else None}")
        print(f"  character: {act.character.decode('utf-8') if act.character else None}")
        print(f"  dialogue: {act.dialogue.decode('utf-8', errors='replace') if act.dialogue else None}")
        
        # 打印 choices
        if act.choices:
            print(f"  choices (n={act.n_choice}):")
            for j in range(act.n_choice):
                choice = act.choices[j]
                print(f"    Choice {j + 1}:")
                print(f"      desc: {choice.desc.decode('utf-8') if choice.desc else None}")
                print(f"      outcome: {choice.outcome.decode('utf-8') if choice.outcome else None}")
        else:
            print("  choices: None")
        
        # 打印 options
        if act.options:
            print(f"  options (n={act.n_option}):")
            for j in range(act.n_option):
                option = act.options[j]
                print(f"    Option {j + 1}:")
                print(f"      text: {option.text.decode('utf-8') if option.text else None}")
                print(f"      next: {option.next.decode('utf-8') if option.next else None}")
        else:
            print("  options: None")

    lib.free_Acts(acts, n_act)



if __name__ == '__main__':
    main()
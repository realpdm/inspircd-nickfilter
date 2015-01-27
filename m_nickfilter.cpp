
#include "inspircd.h"
#include <pcrecpp.h>
#include <sstream> 

/* $CompileFlags: exec("pcre-config --cflags") */
/* $LinkerFlags: exec("pcre-config --libs") rpath("pcre-config --libs") -lpcrecpp */
/* $ModDesc: Provides pre-filtering nickname changes */

class ModuleNickFilter : public Module
{
    
public:
  void init()
  {
    //ServerInstance->SNO->EnableSnomask('n',"NICK");
    Implementation eventlist[] = { I_OnUserPreNick , I_OnUserRegister};
    ServerInstance->Modules->Attach(eventlist, this, 2);
  }

  virtual Version GetVersion()
  {
    return Version("Requires that nicknames be a variation of auth/ident name", VF_VENDOR);
  }

  ModResult OnUserPreNick(User* user, const std::string &newnick)
  {

    std::string oldnick = user->nick;
    std::string ident = user->ident;

        // if no ident is found, we are too early in the connection. We'll fix the nickname OnUserRegister
    if (ident.compare("unknown") == 0) {
      return MOD_RES_PASSTHRU;
    }

    if (ident.compare("webchat") == 0) {
      return MOD_RES_PASSTHRU;
    }

    std::string match_re = "^" + ident ;

    pcrecpp::RE_Options opt;
    opt.set_caseless(true);

    if (pcrecpp::RE(match_re,opt).PartialMatch(newnick)) {
      return MOD_RES_PASSTHRU;
    } else {
      user->WriteServ("NOTICE NICK '%s' is not a valid nickname. Please use a variation of '%s' like '%s_foo'", newnick.c_str(), ident.c_str(), ident.c_str() );
      return MOD_RES_DENY;

    }        
    return MOD_RES_PASSTHRU;
  }


  ModResult OnUserRegister(LocalUser* user)
  {

    if (user->ident.compare("webchat") == 0) {
      return MOD_RES_PASSTHRU;
    }

    std::string match_re = "^" + user->ident ;

    pcrecpp::RE_Options opt;
    opt.set_caseless(true);

    if (pcrecpp::RE(match_re,opt).PartialMatch(user->nick)) {
      return MOD_RES_PASSTHRU;
    } else {
      user->WriteServ("NOTICE NICK '%s' is not a valid nickname. Please use a variation of '%s' like '%s_foo'", user->nick.c_str(), user->ident.c_str(), user->ident.c_str() );


      if (ServerInstance->FindNickOnly(user->ident) ) {
        std::string newnick = user->ident;
        std::stringstream concat;
        for (int i=1; i<10; i++) {
          concat.str(std::string());
          concat << user->ident << "_" << i;
          newnick = concat.str();
          if (!ServerInstance->FindNickOnly(newnick)) {
            user->ChangeNick(newnick,true);
            break;
          } 
        }
      } else {
        user->ChangeNick(user->ident,true);
      }
      return MOD_RES_PASSTHRU;
    }                
  }
};

MODULE_INIT(ModuleNickFilter)

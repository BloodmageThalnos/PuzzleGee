#pragma once

#ifndef NO_LOG
#define LOG_TO_STDOUT
#endif

#ifdef LOG_TO_STDOUT
#define LOG(x) (cout<<(x)<<endl)
#endif
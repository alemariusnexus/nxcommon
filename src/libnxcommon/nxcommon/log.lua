local ffi = require("ffi")

local log = {}

log.LOG_LEVEL_NONE = ffi.C.LOG_LEVEL_NONE
log.LOG_LEVEL_ERROR = ffi.C.LOG_LEVEL_ERROR
log.LOG_LEVEL_WARNING = ffi.C.LOG_LEVEL_WARNING
log.LOG_LEVEL_INFO = ffi.C.LOG_LEVEL_INFO
log.LOG_LEVEL_DEBUG = ffi.C.LOG_LEVEL_DEBUG
log.LOG_LEVEL_VERBOSE = ffi.C.LOG_LEVEL_VERBOSE

function log.SetLogLevel(level)
    ffi.C.SetLogLevel(level)
end

function log.GetLogLevel()
    return ffi.C.GetLogLevel()
end

function log.GetLogLevelName(level)
    return ffi.C.GetLogLevelName(level)
end

function log.IsLogLevelActive(level)
    return ffi.C._LuaIsLogLevelActive(level)
end

function log.LogMessage(level, fmt, ...)
    ffi.C._LuaLogMessagev(level, "%s", string.format(fmt, ...))
end

function log.LogError(fmt, ...) log.LogMessage(log.LOG_LEVEL_ERROR, fmt, ...) end
function log.LogWarning(fmt, ...) log.LogMessage(log.LOG_LEVEL_WARNING, fmt, ...) end
function log.LogInfo(fmt, ...) log.LogMessage(log.LOG_LEVEL_INFO, fmt, ...) end
function log.LogDebug(fmt, ...) log.LogMessage(log.LOG_LEVEL_DEBUG, fmt, ...) end
function log.LogVerbose(fmt, ...) log.LogMessage(log.LOG_LEVEL_VERBOSE, fmt, ...) end


-- Aliases without the 'Log' prefix, allowing the shorter log.Error("...") instead of log.LogError("...")
log.Message = log.LogMessage
log.Error = log.LogError
log.Warning = log.LogWarning
log.Info = log.LogInfo
log.Debug = log.LogDebug
log.Verbose = log.LogVerbose

return log

#pragma once
//
// Note — 一条笔记的纯业务数据。它不是 MVVM Model；NotesModel 才是模块级共享状态中心。
// 一条笔记对应 notes/<id>.md，正文中的图片使用相对路径，便于 Git 同步。
//
#include <string>
#include <cstdint>

namespace wb::notes {

struct Note {
    std::string id;         ///< UUID（= 文件名）
    std::string title;
    std::string body;       ///< markdown 正文（图文）
    std::int64_t updatedAt = 0;  ///< 更新时间戳（秒）
};

}  // namespace wb::notes

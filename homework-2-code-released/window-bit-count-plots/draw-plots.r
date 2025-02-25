library("ggplot2")
# install.packages("dplyr")
library("dplyr")

df <-
  read.table("results.txt",
             col.names=c("algo", "wnd_sz", "throughput", "memory"),
             skip=0, header=FALSE, sep=" ")
head(df)

df_grouped <- df %>%
  #filter(algo != "exact") %>%
  group_by(algo, wnd_sz) %>%
  summarize(t_mean = mean(throughput),
            t_sd = sd(throughput),
            m_mean = mean(memory),
            m_sd = sd(memory)) %>%
  mutate(t_min = t_mean - t_sd,
         t_max = t_mean + t_sd,
         m_min = m_mean - m_sd,
         m_max = m_mean + m_sd)

ggplot(df_grouped, aes(x=as.factor(wnd_sz), y=t_mean, color=factor(algo))) +
  scale_y_continuous(trans='log10') +
  geom_line() + 
  geom_errorbar(aes(ymin=t_min, ymax=t_max), width=0.3) +
  #theme_bw() +
  xlab("window size") +
  ylab("throughput in items/sec") +
  guides(color=guide_legend(title="algorithm")) +
  geom_point()

ggsave("throughput.pdf", width=8, height=6)

ggplot(df_grouped, aes(x=as.factor(wnd_sz), y=m_mean, color=factor(algo))) +
  scale_y_continuous(trans='log10') +
  geom_line() + 
  #geom_errorbar(aes(ymin=m_min, ymax=m_max), width=0.3) +
  #theme_bw() +
  xlab("window size") +
  ylab("memory footprint in bytes") +
  guides(color=guide_legend(title="algorithm")) +
  geom_point()

ggsave("memory_footprint.pdf", width=8, height=6)
